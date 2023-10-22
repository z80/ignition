extends Control

const query_path: String = ".location_query"

@export var DestinationIndicator: PackedScene = null

var QueryState = {
	QUERY_IDLE = 0, 
	QUERY_EXPECT_RESPONSE = 1
}

var _query_state: int = QueryState.QUERY_IDLE


var _query_label: LineEdit = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_query_label = get_node( "Panel/VBox/HBox/LineEdit" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass





func _input(_event):
	if Input.is_action_just_pressed( "ui_enter" ):
		#_create_test_destination()
		#self.queue_free()
		
		_on_query_request()
	
	elif Input.is_action_just_pressed( "ui_escape" ):
		self.queue_free()





func _create_test_destination():
	var dest: Control = DestinationIndicator.instantiate()
	var root: Control = RootScene.get_root_for_gui_windows()
	root.add_child( dest )
	
	dest.camera_rf = RootScene.ref_frame_root.player_camera
	var dest_se3: Se3Ref = Se3Ref.new()
	dest_se3.r = Vector3( 0.0, 100.0, 0.0 )
	dest.destination_se3 = dest_se3




func _on_query_request():
	var dir: DirAccess = DirAccess.open( "res://" )
	var exists: bool = dir.dir_exists( query_path )
	if not exists:
		dir.make_dir_recursive( query_path )
	
	var file_name: String = "res://" + query_path + "/request.json"
	var f: FileAccess = FileAccess.open( file_name, FileAccess.WRITE )
	var stri: String = _query_label.text
	f.store_string( stri )
	f.close()
	
	_query_state = QueryState.QUERY_EXPECT_RESPONSE


func _on_query_check_response():
	var dir: DirAccess = DirAccess.open( "res://" )
	var exists: bool = dir.dir_exists( query_path )
	if not exists:
		dir.make_dir_recursive( query_path )
	
	var file_name: String = "res://" + query_path + "/response.json"
	var f: FileAccess = FileAccess.open( file_name, FileAccess.READ )
	if f == null:
		return
	
	var stri: String = f.get_as_text()
	f.close()
	
	var data = JSON.parse_string( stri )
	var location: Vector3
	location.x = data[0]
	location.y = data[1]
	location.z = data[2]
	
	var err: int = dir.remove( file_name )
	
	# Create the destination indicator.
	var dest: Control = DestinationIndicator.instantiate()
	var root: Control = RootScene.get_root_for_gui_windows()
	root.add_child( dest )
	
	dest.camera_rf = RootScene.ref_frame_root.player_camera
	var dest_se3: Se3Ref = Se3Ref.new()
	dest_se3.r = location
	dest.destination_se3 = dest_se3
	
	_query_state = QueryState.QUERY_IDLE
	
	self.queue_free()


func _on_timer_timeout():
	if _query_state == QueryState.QUERY_EXPECT_RESPONSE:
		_on_query_check_response()
