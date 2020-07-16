extends Node


const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
var DB_NAME = "res://mm_data.sqlite3.db"
var _db = null
const TABLE_NAME = "data"
var _frames_qty: int = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	open_database()

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func open_database():
	_db = SQLite.new()
	_db.path = DB_NAME
	var res: bool = _db.open_db()
	
	if not res:
		print( "Failed to open frame database ", DB_NAME )
		return false
		
	_db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_NAME + ";")
	_frames_qty = _db.query_result[0]['qty']
	
	return true

func build_kd_tree():
	pass


