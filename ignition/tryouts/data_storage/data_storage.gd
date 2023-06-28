extends Node

const root_folder_name: String = "res://.storage/"

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func serialize( collision_surface_one: Node, data: Dictionary ):
	var rf: RefFrameNonInertialNode = collision_surface_one.get_parent()
	var rot: RefFrameRotationNode   = rf.get_parent()
	var tr: RefFrameMotionNode      = rot.get_parent()
	var path: String = tr.get_path()
	var bounding_node: BoundingNodeGd = collision_surface_one.bounding_node
	if bounding_node == null:
		return false
	
	var id: String = bounding_node.get_node_id()
	path = root_folder_name + path
	var dir: DirAccess = DirAccess.open("res://")
	var exists: bool = dir.dir_exists( path )
	if not exists:
		dir.make_dir_recursive( path )
	
	var file_name: String = path + "/" + id + ".json"
	var file: FileAccess = FileAccess.open( file_name, FileAccess.WRITE )
	var data_string: String = JSON.stringify( data, "    ", true, true )
	file.store_string( data_string )
	file.close()
	return true


func deserialize( collision_surface_one: Node ):
	var rf: RefFrameNonInertialNode = collision_surface_one.get_parent()
	var rot: RefFrameRotationNode   = rf.get_parent()
	var tr: RefFrameMotionNode      = rot.get_parent()
	var path: String = tr.get_path()
	var bounding_node: BoundingNodeGd = collision_surface_one.bounding_node
	var id: String = bounding_node.get_node_id()
	path = root_folder_name + path
	var file_name: String = path + "/" + id + ".json"
	var exists: bool = FileAccess.file_exists( file_name )
	if not exists:
		return null
	
	var file: FileAccess = FileAccess.open( file_name, FileAccess.READ )
	var data_string: String = file.get_as_text()
	var data: Dictionary = JSON.parse_string( data_string )
	return data


