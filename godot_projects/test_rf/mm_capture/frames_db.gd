
extends Node
class_name FramesDb

const TABLE_DATA_NAME   = "data"
const TABLE_CONFIG_NAME = "config"

export( String) var file_name = "res://mm_vive_capture.sqlite3.db"

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
var _db = null

var _frames_qty: int = 0


func _ready():
	open()


func _exit_tree():
	close()


func open():
	if _db != null:
		_db.close_db()
	
	var ok: bool = _open_frame_database( file_name )
	if not ok:
		return false
	_query_frames_qty()
	
	return true


func close():
	_db.close_db()
	_db = null


func _open_frame_database( fname: String = "res://mm_data.sqlite3.db" ):
	var db = SQLite.new()
	db.path = fname
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", fname )
		return false
		
	res = db.query("CREATE TABLE IF NOT EXISTS data (id integer PRIMARY KEY, data text, desc text, cat text);")
	if not res:
		print( "ERROR: Failed to create \'data\' table" )
		return

	res = db.query("CREATE TABLE IF NOT EXISTS config (id string PRIMARY KEY, data text);")
	if not res:
		print( "ERROR: Failed to create \'config\' table" )
		return
	
	_db = db
	
	return true


func _query_frames_qty():
	_db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	_frames_qty = _db.query_result[0]['qty']
	return _frames_qty


func set_config( key: String, data ):
	var stri: String = JSON.print( data )
	stri = stri.replace( "\"", "\'" )
	var cmd: String = "INSERT OR REPLACE INTO config(id, data) VALUES (\'%s\', \'%s\');" % [key, stri]
	var res: bool = _db.query( cmd )
	return res


func get_config( key: String ):
	var cmd: String = "SELECT data FROM config WHERE id=\'%s\' LIMIT 1;" % key
	#var cmd: String = "SELECT FROM config(id, data);"
	var res: bool = _db.query( cmd )
	if not res:
		print( "config query failed" )
		return null
	var q_res: Array = _db.query_result
	if q_res.size() < 1:
		return null
	var stri: String = q_res[0]['data']
	stri = stri.replace( "\'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret


func get_frame( index: int ):
	var data = _query_data( index, "data" )
	return data


func set_frame( index: int, data ):
	var ok: bool = _set_data( index, "data", data )
	return ok


func get_desc( index ):
	var data = _query_data( index, "desc" )
	return data


func set_desc( index: int, data ):
	var ok: bool = _set_data( index, "desc", data )
	return ok


func asign_default_category( cat ):
	var stri_d: String = JSON.print( cat )
	stri_d = stri_d.replace( "\"", "\'" )
	var cmd = "UPDATA data SET cat=\'%s\';" % stri_d
	var ok: bool = _db.query( cmd )
	return ok

func assign_category( start_index: int, end_index: int, cat ):
	var stri_d: String = JSON.print( cat )
	stri_d = stri_d.replace( "\"", "\'" )
	var cmd = "UPDATA data SET cat=\'%s\' WHERE id>=%d AND id<=%d;" % [stri_d, start_index, end_index]
	var ok: bool = _db.query( cmd )
	return ok


func get_category( index: int ):
	var cat = _query_data( index, "cat" )
	return cat


func _set_data( index: int, column_name: String, data ):
	var stri_d: String = JSON.print( data )
	stri_d = stri_d.replace( "\"", "\'" )
	#var cmd = "INSERT OR REPLACE INTO data(id, data, desc) VALUES(%d, \'%s\', \'%s\');" % [i, stri_f, stri_d]
	var cmd: String = "INSERT OR IGNORE INTO data ( id, %s ) VALUES( %d, \'%s\' );" % [ column_name, index, stri_d ]
	var res: bool = _db.query( cmd )
	if not res:
		return false
	cmd = "UPDATE data SET %s=\'%s\' WHERE id=%d;" % [column_name, stri_d, index]
	res = _db.query( cmd )
	return res


func _query_data( index: int, column_name: String ):
	var cmd: String = "SELECT %s FROM data WHERE id = %d LIMIT 1;" % [column_name, index]
	var res: bool = _db.query( cmd )
	if not res:
		print( "failed to query data from the db at index ", index )
		return null
	var selected_array : Array = _db.query_result
	var stri = selected_array[0]['data']
	stri = stri.replace( "'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret






