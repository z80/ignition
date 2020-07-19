extends Node


const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
var DB_NAME = "res://mm_data.sqlite3.db"
var db_ = null
const TABLE_NAME = "data"
var frames_qty_: int = -1
var frame_search_ = null


# Called when the node enters the scene tree for the first time.
func _ready():
	open_database()
	build_kd_tree()
	
	#for i in range( 3 ):
	#	var f = frame_( i * 300 )
	#	print( f )
	#	print( "-------" )

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func open_database():
	db_ = SQLite.new()
	db_.path = DB_NAME
	var res: bool = db_.open_db()
	
	if not res:
		print( "Failed to open frame database ", DB_NAME )
		return false
		
	db_.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_NAME + ";")
	frames_qty_ = db_.query_result[0]['qty']
	
	return true


func frame_( index: int ):
	var select_condition: String = "id = %d" % index
	var selected_array : Array = db_.select_rows('data', select_condition, ['data'])
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


func build_kd_tree():
	print( "started" )
	
	var all: Array
	var selected_array : Array = db_.select_rows('data', "", ['id', 'data'])
	for v in selected_array:
		var ind: int     = v['id']
		var data: String = v['data']
		data = data.replace( "'", "\"" )
		#print( "raw data: ", stri )
		var res = JSON.parse( data )
		res = res.result
		all.push_back( res )
	
	print( "done" )
	return
	
	frame_search_ = FrameSearch.new()
	var progress: int = 0
	print( "Reading %d frames" % frames_qty_ )
	for i in range( frames_qty_ ):
		var frame = frame_(i)
		var p: int = 10 * (i+1) / frames_qty_
		if p != progress:
			progress = p
			p *= 10
			print( "progress ", p, "%" )


