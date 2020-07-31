
extends Node
class_name MmSaver

const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const DB_NAME = "res://mm_dump.sqlite3.db"

var _bone_names: Dictionary
var _db = null
var _index: int = 0

func init():
	_bone_names = {}
	_index = 0
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
		_db.close()
		_db = null



func set_bone_names( names: Array, names_map: Dictionary ):
	_bone_names = {}
	var names_subset: Array = names_map.keys()
	for name in names_subset:
		if name in names:
			var ind: int = names.find( name )
			var target_name: String = names_map[name]
			_bone_names[ind] = target_name


func store( frame ):
	var data = {}
	var gnd: Array = [false, false]
	data['gnd'] = gnd
	var links: Dictionary = {}
	for ind in _bone_names:
		var name: String = _bone_names[ind]
		var i = ind * 7
		var x: float  = frame[i+4]
		var y: float  = frame[i+5]
		var z: float  = frame[i+6]
		var qw: float = frame[i]
		var qx: float = frame[i+1]
		var qy: float = frame[i+2]
		var qz: float = frame[i+3]
		var link: Dictionary = { 'x': x, 'y': y, 'z': z, 'qw': qw, 'qx': qx, 'qy': qy, 'qz': qz }
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




