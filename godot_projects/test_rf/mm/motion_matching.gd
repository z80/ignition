extends Node


const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const FRAME_DB_NAME = "res://mm_data.sqlite3.db"
const DESC_DB_NAME  = "res://desc_mm_data.sqlite3.db"
var db_ = null
const TABLE_DATA_NAME = "data"
const TABLE_CONFIG_NAME = "config"
var frames_qty_: int = -1
var frame_search_ = null
var bone_names_ = null

const ROOT_IND: int       = 0
const LEFT_LEG_IND: int   = 1
const RIGHT_LEG_IND: int  = 2
const LEFT_HAND_IND: int  = 3
const RIGHT_HAND_IND: int = 5
const POSE_LIMB_INDS: Array = [ROOT_IND, LEFT_LEG_IND, RIGHT_LEG_IND, LEFT_HAND_IND, RIGHT_HAND_IND]
const TRAJ_FRAME_INDS: Array = [30, 60, 90, 120]

const FPS: float = 60.0
const DT: float  = 1.0/FPS


# Called when the node enters the scene tree for the first time.
func _ready():
	var db = open_frame_database()
	build_kd_tree( db )
	
	#for i in range( 3 ):
	#	var f = frame_( i * 300 )
	#	print( f )
	#	print( "-------" )

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func open_frame_database():
	var db = SQLite.new()
	db.path = FRAME_DB_NAME
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", FRAME_DB_NAME )
		return null
		
	db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	frames_qty_ = db.query_result[0]['qty']
	
	# Read bone names.
	var selected_array: Array = db.select_rows( TABLE_CONFIG_NAME, "id = \'names\'", ['data'] )
	if selected_array.size() < 1:
		return false
	var stri: String = selected_array[0]['data']
	stri = stri.replace( "'", "\"" )
	bone_names_ = JSON.parse( stri )
	
	return db


func create_desc_database():
	var db = SQLite.new()
	db.path = DESC_DB_NAME
	var res: bool = db_.open_db()
	
	if not res:
		print( "Failed to open frame database ", DESC_DB_NAME )
		return false
	
	# Create table for frame data and descriptors.
	var table_dict : Dictionary = Dictionary()
	table_dict["id"]   = {"data_type":"int", "primary_key": true, "not_null": true}
	table_dict["pose"] = {"data_type":"text", "not_null": true}
	table_dict["desc"] = {"data_type":"text", "not_null": true}
	# Throw away any table that was already present
	db.drop_table( 'frame' )
	# Create a table with the structure found in table_dict and add it to the database
	db.create_table( 'frame', table_dict )
	
	
	# Create table for configuration.
	# Currently bone names are stored here.
	table_dict.clear()
	table_dict["id"]   = {"data_type":"text", "primary_key": true, "not_null": true}
	table_dict["data"] = {"data_type":"text", "not_null": true}
	db.drop_table( 'config' )
	# Create a table with the structure found in table_dict and add it to the database
	db.create_table( 'config', table_dict )

	return true


func fill_desc_database( frame_db, desc_db ):
	# I'm here now.....
	pass



func set_config_( db, key: String, data ):
	var stri: String = JSON.print( data )
	stri = stri.replace( "\"", "\'" )
	var cmd: String = "INSERT OR REPLACE INTO config(id, data) VALUES (\'%s\', \'%s\');" % [key, stri]
	var res: bool = db.query( cmd )
	return res


func get_config_( db, key: String ):
	var cmd: String = "SELECT DISTINCT from config(data) WHERE id=\'%s\'" % key
	db.query( cmd )
	var stri: String = db.query_result[0]['data']
	stri = stri.replace( "\'", "\"" )
	var res = JSON.parse( stri )
	res = res.result
	return res



func frame_( db, index: int ):
	var select_condition: String = "id = %d" % index
	var selected_array : Array = db.select_rows('data', select_condition, ['data'])
	var stri = selected_array[0]['data']
	stri = stri.replace( "'", "\"" )
	#print( "raw data: ", stri )
	var res = JSON.parse( stri )
	#var err = res.error
	#print( "JSON parse error line: ", err )
	#var err_line = res.error_line
	#print( "JSON parse error line: ", err_line )
	#print( "result: ", res.result )
	#print( "gnd type: ", typeof( res.result['gnd'][0] ) == TYPE_BOOL )
	res = res.result
	return res


func build_kd_tree( db ):
	print( "Reading frames database..." )
	frame_search_ = FrameSearch.new()
	frame_search_.set_dims( 18 )
	
	var selected_array : Array = db.select_rows('data', "", ['id', 'data'])
	for v in selected_array:
		var ind: int     = v['id']
		var data: String = v['data']
		data = data.replace( "'", "\"" )
		#print( "raw data: ", stri )
		var res = JSON.parse( data )
		res = res.result
		var desc = res["desc"]
		frame_search_.append( desc )
	
	print( "done" )
	
	print( "Building KdTree..." )
	frame_search_.build_tree()
	print( "done" )


func pose_desc_( db, index: int ):
	var f = frame_( db, index )
	var fp
	if ( index > 0 ):
		fp = frame_( db, index-1 )
	else:
		fp = f
	
	# Root pose
	var root_q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	var root_r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	var root_q_inv = root_q.inverse()
	
	var desc_r: Array
	var desc_v: Array
	for limb_ind in POSE_LIMB_INDS:
		var ind: int = limb_ind * 7
		var q: Quat = Quat( f[ind+1], f[ind+12], f[ind+13], f[ind+10] )
		var r: Vector3 = Vector3( f[ind+4], f[ind+5], f[ind+6] )
		#var qp: Quat = Quat( fp[ind+1], fp[ind+12], fp[ind+13], fp[ind+10] )
		var rp: Vector3 = Vector3( fp[ind+4], fp[ind+5], fp[ind+6] )
		r  -= root_r
		rp -= root_r
		r  = root_q_inv.xform( r )
		rp = root_q_inv.xform( r )
		#q  = root_q_inv * q
		#qp = root_q_inv * qp
		var v: Vector3 = (r - rp) * FPS
		if ( limb_ind != ROOT_IND ):
			desc_r += [ r.x, r.y ]
		desc_v += [ v.x, v.y ]
		
	# Also compute Z in local ref. frame. It defines torso tilt.
	var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	g = root_q_inv.xform( g )
	var desc_g: Array = [ g.x, g.y ]
		
	return [ desc_r, desc_v, desc_g ]


func traj_desc_( db, index: int ):
	var f = frame_( db, index )
	
	# Root pose
	var root_q: Quat = Quat( f[ROOT_IND+1], f[ROOT_IND+2], f[ROOT_IND+3], f[ROOT_IND] )
	var root_r: Vector3 = Vector3( f[ROOT_IND+4], f[ROOT_IND+5], f[ROOT_IND+6] )
	var root_q_inv = root_q.inverse()
	
	var desc_r: Array
	var desc_az: Array
	var desc_g: Array
	
	for ind in TRAJ_FRAME_INDS:
		var frame_ind: int = index + ind
		if frame_ind >= frames_qty_:
			frame_ind = frames_qty_ - 1
			var fn = frame_( db, frame_ind )
			var q: Quat = Quat( fn[ROOT_IND+1], fn[ROOT_IND+2], fn[ROOT_IND+3], fn[ROOT_IND] )
			var r: Vector3 = Vector3( fn[ROOT_IND+4], fn[ROOT_IND+5], fn[ROOT_IND+6] )
			var q_inv: Quat = q.inverse()
			r = r - root_r
			r = root_q_inv.xform( r )
			var d = [r.x, r.y]
			desc_r += d
			
			var fwd: Vector3 = Vector3( 1.0, 0.0, 0.0 )
			fwd = q.xform( fwd )
			fwd = root_q_inv.xform( fwd )
			d = [fwd.x, fwd.y]
			desc_az += d
			
			var g: Vector3 = Vector3( 0.0, 0.0, 1.0 )
			g =	q_inv.xform( g )
			d = [g.x, g.y]
			desc_g += d
			
	return [desc_r, desc_az, desc_g]
		
		


