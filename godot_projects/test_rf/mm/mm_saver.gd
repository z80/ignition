
extends Node
class_name MmSaver

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const DB_NAME = "res://mm_dump.sqlite3.db"

var _bone_names: Dictionary
var _db = null
var _index: int = 0
var _mm_frame_ind: int = -1

var _azimuth_initialized: bool = false
var _inv_az_q: Quat = Quat.IDENTITY

# Previous frame. This is for running interpolation.
var _frame_prev = null

# For determining "on ground" state.
const LEFT_FOOT_IND:  int = 81
const RIGHT_FOOT_IND: int = 86
const FOOT_HEIGHT_DIFF: float = 0.03

# Origin FPS is somewhat low. Increasing it by duplicating frames.
const FPS_MULTIPLIER: int = 4

var _feet = null

func init():
	_index = 0
	_mm_frame_ind = -1
	_azimuth_initialized = false
	_frame_prev = null
	_feet = _init_feet()
	_db = _open_database()

func _init_feet():
	var l_ft = {"th": 10, "prev": 0.0, "count": 10, "ind": LEFT_FOOT_IND}
	var r_ft = {"th": 10, "prev": 0.0, "count": 10, "ind": RIGHT_FOOT_IND}
	var feet = [l_ft, r_ft]
	return feet


func _open_database( fname = DB_NAME ):
	var db = SQLite.new()
	db.path = DB_NAME
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", DB_NAME )
		return null
		
	res = db.query("CREATE TABLE IF NOT EXISTS data (id integer PRIMARY KEY, data text NOT NULL);")
	if not res:
		print( "ERROR: Failed to create \'data\' table" )
		return

	res = db.query("CREATE TABLE IF NOT EXISTS config (id string PRIMARY KEY, data text NOT NULL);")
	if not res:
		print( "ERROR: Failed to create \'config\' table" )
		return
	
	_index = 0
	
	return db


func close():
	if _db != null:
		_db.close_db()
		_db = null



func set_names_and_quats( names_and_quats_map: Dictionary ):
	_bone_names = names_and_quats_map


func _compute_gnd( frame ):
	var li: int = LEFT_FOOT_IND*7 + 6
	var ri: int = RIGHT_FOOT_IND*7 + 6
	var lh: float = frame[li]
	var rh: float = frame[ri]
	var gnd = [false, false]
	var diff: float = abs( lh - rh )
	if diff < FOOT_HEIGHT_DIFF:
		gnd[0] = true
		gnd[1] = true
	else:
		if lh < rh:
			gnd[0] = true
		else:
			gnd[1] = true
	return gnd


func _in_region( x, y, th = 0.002 ):
	if (x >= y-th) and (x <= y+th):
		return true
	return false


func _on_gnd2( frame, left: bool = true, init: bool = false ):
	var ft
	if left:
		ft = _feet[0]
	else:
		ft = _feet[1]
	var ind: int = 7 * ft["ind"]
	var z: float = frame[ind + 6]
	
	var contact: bool = false
	
	if init:
		ft["th"] = z + 0.02
		ft["prev"] = z
		ft["count"] = 10
	
	var in_reg = _in_region( z, ft["prev"] )
	if left:
		print( "left in reg:", in_reg, "; z: ", z, ", prev: ", ft["prev"] )
	#else:
	#	print( "right in reg:", in_reg )
		
	if z <= ft["th"]:
		if in_reg:
			ft["count"] += 1
			
			if ft["count"] > 2:
				ft["th"] = z + 0.02
				contact = true
				
				if left:
					print( "contact <- true" )
		else:
			ft["count"] = 0
	
	else:
		ft["count"] = 0
	
	ft["prev"] = z
	
	return contact


func _compute_gnd2( frame, first_frame: bool ):
	var left: bool  = _on_gnd2( frame, true,  first_frame )
	var right: bool = _on_gnd2( frame, false, first_frame )
	var res = [left, right]
	return res


func _compute_azimuth( mm, frame ):
	var root_ind = mm.ROOT_IND * 7
	var q: Quat = Quat( frame[root_ind+1], frame[root_ind+2], frame[root_ind+3], frame[root_ind] )
	q = mm.quat_azimuth_( q )
	return q


func store( mm, frame, frame_ind ):
	if frame_ind == _mm_frame_ind:
		return
	
	# If there is no previous frame, make a deep copy and return as 
	# Ther eis not previous frame for interpolation.
	if _frame_prev == null:
		# Make a deep copy and return.
		var qty: int = frame.size()
		_frame_prev = []
		for i in range( qty ):
			_frame_prev.push_back( frame[i] )
		return
	
	# Two frames to interpolate in between.
	var f0 = _frame_prev
	var f1 = frame
	
	var f: Array
	
	for repeat_ind in range( FPS_MULTIPLIER ):
		var t: float
		if FPS_MULTIPLIER > 1:
			t = float(repeat_ind) / float(FPS_MULTIPLIER-1)
		else:
			t = 1.0
		f = _slerp_frame( f0, f1, t )
		
		_init_azimuth( mm, f )
		
		_mm_frame_ind = frame_ind
		
		var data = {}
		#var gnd: Array = _compute_gnd( f )
		var first_frame: bool = (_index < 1)
		var gnd: Array = _compute_gnd2( f, first_frame )
		#print( "gnd: ", gnd )
		
		var q_r: Quat = _compute_azimuth( mm, f )
		data['q'] = [ q_r.w, q_r.x, q_r.y, q_r.z ]
		
		data['gnd'] = gnd
		var links: Dictionary = {}
		for ind in _bone_names.keys():
			var v = _bone_names[ind]
			var name: String = v["dest"]
			var adj_q: Quat = v["q"]
			var i = ind * 7
			var x: float  = f[i+4]
			var y: float  = f[i+5]
			var z: float  = f[i+6]
			var qw: float = f[i]
			var qx: float = f[i+1]
			var qy: float = f[i+2]
			var qz: float = f[i+3]
			var q: Quat = Quat( qx, qy, qz, qw )
			q = _inv_az_q * q * adj_q
			var link: Dictionary = { 'x': x, 'y': y, 'z': z, 'qw': q.w, 'qx': q.x, 'qy': q.y, 'qz': q.z }
			links[name] = link
		data['links'] = links
		var stri: String = JSON.print( data )
		stri = stri.replace( "\"", "\'\'" )
		
		var cmd_stri: String = "INSERT OR REPLACE INTO data(id, data) VALUES( %d, \'%s\' )" % [ _index, stri ]
		var res: bool = _db.query( cmd_stri )
		if not res:
			print( "ERROR: failed to place frame information into the database" )
			return
			
		_index += 1
	
	# Update previous frame in order to 
	# be able to smoothly interpolate.
	_frame_prev = f



func _init_azimuth( mm, frame ):
	if _azimuth_initialized:
		return
	# Here frame is already compensated for own rotation and azimuth is applied.
	# So it is necessary to apply it's azimuth to (0, -1, 0) vector and compute its atan2(y, z).
	var root_ind: int = mm.ROOT_IND*7
	var q: Quat = Quat( frame[root_ind+1], frame[root_ind+2], frame[root_ind+3], frame[root_ind] )
	var az_q: Quat = mm.quat_azimuth_( q )
	var fwd = az_q.xform( mm.V_HEADING_FWD )
	var ang: float = atan2( fwd.y, fwd.x )
	var ang_2: float = ang / 2.0
	var co2: float = cos(ang_2)
	var si2: float = sin(ang_2)
	_inv_az_q = Quat( 0.0, 0.0, -si2, co2 )
	_azimuth_initialized = true



func _slerp_frame( f0: Array, f1: Array, t: float ):
	var qty: int = f0.size()
	var qty1: int = f1.size()
	if qty1 < qty:
		qty = qty1
	var f = []
	f.resize( qty )
	for i in range(0, qty, 7):
		var q0: Quat = Quat( f0[i+1], f0[i+2], f0[i+3], f0[i] )
		var q1: Quat = Quat( f1[i+1], f1[i+2], f1[i+3], f1[i] )
		var r0: Vector3 = Vector3( f0[i+4], f0[i+5], f0[i+6] )
		var r1: Vector3 = Vector3( f1[i+4], f1[i+5], f1[i+6] )
		var q: Quat    = _slerp_quat( q0, q1, t )
		var r: Vector3 = _slerp_vector3( r0, r1, t )
		f[i]   = q.w
		f[i+1] = q.x
		f[i+2] = q.y
		f[i+3] = q.z
		f[i+4] = r.x
		f[i+5] = r.y
		f[i+6] = r.z
	
	return f


func _slerp_quat( q0: Quat, q1: Quat, t: float ):
	var dot: float = q0.w*q1.w + q0.x*q1.x + q0.y*q1.y + q0.z*q1.z;
	if dot < 0.0:
		q1 = -q1
		dot = -dot;
	
	# If almost the same quats interpolate linearly and 
	# normalize.
	if dot > 0.9995:
		var q: Quat = q0*(1.0-t) + q1*t
		q = q.normalized()
		return q
	
	var theta_0: float     = acos(dot)
	var theta: float       = theta_0 * t
	var sin_theta: float   = sin(theta)
	var sin_theta_0: float = sin(theta_0)
	
	var s0: float = cos(theta) - dot * sin_theta / sin_theta_0;
	var s1: float = sin_theta / sin_theta_0;
	var res: Quat = (q0 * s0) + (q1 * s1)
	return res


func _slerp_vector3( r0: Vector3, r1: Vector3, t: float ):
	var r: Vector3 = r0*(1.0-t) + r1*t
	return r













