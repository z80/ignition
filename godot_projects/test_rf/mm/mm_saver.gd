
extends Node
class_name MmSaver

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const DB_NAME = "res://mm_dump.sqlite3.db"

var _bone_names: Dictionary
var _db = null
var _index: int = 0
var _mm_frame_ind: int = -1

# For determining "on ground" state.
const LEFT_FOOT_IND:  int = 81
const RIGHT_FOOT_IND: int = 86
const FOOT_HEIGHT_DIFF: float = 0.03


func init():
	_index = 0
	_mm_frame_ind = -1
	_db = _open_database()


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


func _compute_azimuth( mm, frame ):
	var root_ind = mm.ROOT_IND * 7
	var q: Quat = Quat( frame[root_ind+1], frame[root_ind+2], frame[root_ind+3], frame[root_ind] )
	q = mm.quat_azimuth_( q )
	return q


func store( mm, frame, frame_ind ):
	if frame_ind == _mm_frame_ind:
		return
	
	_mm_frame_ind = frame_ind
	
	var data = {}
	var gnd: Array = _compute_gnd( frame )
	#print( "gnd: ", gnd )
	
	var q_r: Quat = _compute_azimuth( mm, frame )
	data['q'] = [ q_r.w, q_r.x, q_r.y, q_r.z ]
	
	data['gnd'] = gnd
	var links: Dictionary = {}
	for ind in _bone_names.keys():
		var v = _bone_names[ind]
		var name: String = v["dest"]
		var adj_q: Quat = v["q"]
		var i = ind * 7
		var x: float  = frame[i+4]
		var y: float  = frame[i+5]
		var z: float  = frame[i+6]
		var qw: float = frame[i]
		var qx: float = frame[i+1]
		var qy: float = frame[i+2]
		var qz: float = frame[i+3]
		var q: Quat = Quat( qx, qy, qz, qw )
		q = q * adj_q
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




