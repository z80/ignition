extends Spatial


const SQLite = preload("res://addons/godot-sqlite/bin/gdsqlite.gdns")
const FRAME_DB_NAME = "res://mm_dump.sqlite3.db"
var _db = null
const TABLE_DATA_NAME   = "data"
const TABLE_CONFIG_NAME = "config"
var _frames_qty: int = -1

const FPS: float = 15.0
const DT: float  = 1.0/FPS
var _time: float = 0.0
var _frame_ind: int = 0

var _frame: Dictionary

# Name -> MeshInstance dictionary
var _visual_meshes: Dictionary

var mat_blue = preload("res://mm_replay/blue.tres")
var mat_red  = preload("res://mm_replay/red.tres")

const FOOT_LEFT  = "L_AK_R_link"
const FOOT_RIGHT = "R_AK_R_link"
const FEET = [FOOT_LEFT, FOOT_RIGHT]

# Called when the node enters the scene tree for the first time.
func _ready():
	_init()


func _process( dt ):
	_time += dt
	if _time < DT:
		return
	
	_time -= DT
	_frame_ind += 1
	if _frame_ind >= _frames_qty:
		_frame_ind = 0
	_apply_to_visual()





func _init():
	_frame_ind = 0
	_db = _open_frame_database()
	_frame = _read_frame( _db, _frame_ind )
	_create_visuals( _frame )
	_apply_to_visual()




func _open_frame_database():
	var db = SQLite.new()
	db.path = FRAME_DB_NAME
	var res: bool = db.open_db()
	
	if not res:
		print( "Failed to open frame database ", FRAME_DB_NAME )
		return null
		
	db.query("SELECT COUNT(*) AS 'qty' FROM " + TABLE_DATA_NAME + ";")
	_frames_qty = db.query_result[0]['qty']
	
	return db


func _read_frame( db, index: int ):
	var cmd: String = "SELECT data FROM data WHERE id = %d LIMIT 1;" % index
	var res: bool = db.query( cmd )
	if not res:
		print( "failed to query frame from the db" )
		return null
	var selected_array : Array = db.query_result
	var stri = selected_array[0]['data']
	stri = stri.replace( "'", "\"" )
	var ret = JSON.parse( stri )
	ret = ret.result
	return ret



func _create_visuals( frame ):
	# Cleanup previous run (if any).
	var children = get_children()
	for ch in children:
		var mi = ch as MeshInstance
		if mi == null:
			continue
		ch.queue_free()
	_visual_meshes.clear()
	
	var mesh = SphereMesh.new()
	
	# Create meshes and fill in the dictionary.
	var links = frame["links"]
	for name in links:
		var link = links[name]
		var mesh_inst: MeshInstance = MeshInstance.new()
		mesh_inst.name = name
		add_child( mesh_inst )
		mesh_inst.mesh = mesh
		mesh_inst.scale = Vector3( 0.05, 0.05, 0.05 )
		
		_visual_meshes[name] = mesh_inst



func _apply_to_visual():
	_frame = _read_frame( _db, _frame_ind )
	var links = _frame["links"]
	for name in _visual_meshes:
		var mi = _visual_meshes[name]
		var p  = links[name]
		var r: Vector3 = Vector3( p["x"], p["z"], -p["y"] )
		var q: Quat    = Quat( p["qx"], p["qz"], -p["qy"], p["qw"]  )
		var t: Transform
		t.origin = r
		t.basis  = q
		mi.transform = t
		mi.scale  = Vector3( 0.05, 0.05, 0.05 )
		var qty = mi.get_surface_material_count()
		if qty < 1:
			continue
		if not (name in FEET):
			mi.set_surface_material( 0, mat_blue )
		else:
			if (name == FOOT_LEFT):
				if (_frame["gnd"][0]):
					mi.set_surface_material( 0, mat_red )
				else:
					mi.set_surface_material( 0, mat_blue )
			elif (name == FOOT_RIGHT):
				if (_frame["gnd"][1]):
					mi.set_surface_material( 0, mat_red )
				else:
					mi.set_surface_material( 0, mat_blue )
				
		




