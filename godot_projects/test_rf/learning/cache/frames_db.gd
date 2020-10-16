
extends Node
class_name FramesDb

export( String) var file_name = "res://anim.db"

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
var _db = null

var frames_qty: int = 0

# These are for caching functionality
export(int) var size = 1024
var queue_data: Array = []
var dict_data: Dictionary = {}
var queue_rf: Array = []
var dict_rf: Dictionary = {}
# Cache forward qty
# When the key is not in the cache it reads these number of items forward
# And places to the cache.
export(int) var cache_forward_qty = 32

# Two cache functions

# Cache query
func _cache_query( key, queue: Array, dict: Dictionary ):
	var exists = dict.has( key )
	if not exists:
		return null
	var p = dict[key]
	var v = p[1]
	return v

func _cache_insert( key, value, queue: Array, dict: Dictionary ):
	# Limit the size
	var sz = queue.size()
	while sz >= size:
		var n = queue.back()
		var k = n[0]
		dict.erase( k )
		queue.pop_back()
		sz -= 1
	# Also make sure the same key doesn't exist
	var exists = dict.has( key )
	if exists:
		var p = dict[key]
		queue.erase( p )
		dict.erase( key )
	# Insert new value
	var v = [ key, value ]
	queue.push_front( v )
	dict[key] = v




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
	if _db != null:
		_db.close_db()
		_db = null


func _open_frame_database( fname: String = "res://anim.db" ):
	var db = SQLite.new()
	db.path = fname
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", fname )
		return false
		
	res = db.query("CREATE TABLE IF NOT EXISTS data (id integer PRIMARY KEY, data text, desc text, cat text, rf text);")
	if not res:
		print( "ERROR: Failed to create \'data\' table" )
		return

	res = db.query("CREATE TABLE IF NOT EXISTS config (id string PRIMARY KEY, data text);")
	if not res:
		print( "ERROR: Failed to create \'config\' table" )
		return
	
	# Again make sure "rf" column exists.
	var cmd: String = "ALTER TABLE data ADD COLUMN rf text;"
	res = db.query( cmd )

	
	_db = db
	
	return true


func _query_frames_qty():
	_db.query("SELECT COUNT(*) AS 'qty' FROM data;")
	frames_qty = _db.query_result[0]['qty']
	return frames_qty


func cleanup():
	var cmd_data: String = "DELETE FROM data;"
	var res_data: bool = _db.query( cmd_data )
	
	var cmd_config: String = "DELETE FROM config;"
	var res_config: bool = _db.query( cmd_config )

	var res: bool = cmd_data and cmd_config
	return res


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
	var data = _cache_query( index, queue_data, dict_data )
	if data == null:
		data = _query_data_bulk( index, "data", queue_data, dict_data )
	return data


func set_frame( index: int, data ):
	var ok: bool = _set_data( index, "data", data )
	if ok:
		_cache_insert( index, data, queue_data, dict_data )
	return ok


func get_desc( index ):
	var data = _query_data( index, "desc" )
	return data


func set_desc( index: int, data ):
	var ok: bool = _set_data( index, "desc", data )
	return ok


func set_rf( index: int, q: Quat, r: Vector3 ):
	var data = [q.w, q.x, q.y, q.z, r.x, r.y, r.z]
	var ok: bool = _set_data( index, "rf", data )
	if ok:
		_cache_insert( index, data, queue_rf, dict_rf )
	return ok



func get_rf( index: int ):
	var data = _cache_query( index, queue_rf, dict_rf )
	if data == null:
		data = _query_data_bulk( index, "rf", queue_rf, dict_rf )
	var quat: Quat = Quat( data[1], data[2], data[3], data[0] )
	var at: Vector3 = Vector3( data[4], data[5], data[6] )
	data = { q=quat, r=at }
	return data



func assign_default_category( cat ):
	var stri_d: String = JSON.print( cat )
	stri_d = stri_d.replace( "\"", "\'\'" )
	var cmd = "UPDATE data SET cat=\'%s\';" % stri_d
	var ok: bool = _db.query( cmd )
	return ok

func assign_category( start_index: int, end_index: int, cat ):
	var stri_d: String = JSON.print( cat )
	stri_d = stri_d.replace( "\"", "\'\'" )
	var cmd = "UPDATE data SET cat=\'%s\' WHERE id>=%d AND id<=%d;" % [stri_d, start_index, end_index]
	var ok: bool = _db.query( cmd )
	return ok


func get_category( index: int ):
	var cat = _query_data( index, "cat" )
	return cat


func _set_data( index: int, column_name: String, data ):
	var stri_d: String = JSON.print( data )
	stri_d = stri_d.replace( "\"", "\'\'" )
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
	var stri = selected_array[0][column_name]
	stri = stri.replace( "\'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret


func _query_data_bulk( index: int, column_name: String, queue: Array, dict: Dictionary ):
	var ind_from: int = index
	var ind_to: int = index + cache_forward_qty
	if ind_to >= frames_qty:
		ind_to = frames_qty - 1
	
	var cmd: String = "SELECT %s FROM data WHERE id >= %d AND id <= %d;" % [column_name, ind_from, ind_to]
	var res: bool = _db.query( cmd )
	if not res:
		print( "failed to query data from the db at index ", index )
		return null
	var selected_array : Array = _db.query_result
	
	var result = null
	var qty = ind_to - ind_from + 1
	for i in range(qty):
		var ind = ind_from + i
		var stri = selected_array[i][column_name]
		stri = stri.replace( "\'", "\"" )
		var ret = JSON.parse( stri )
		ret = ret.result
		_cache_insert( ind, ret, queue, dict )
		if result == null:
			result = ret
	return result


