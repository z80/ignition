
extends "res://physics/ref_frames/root/ref_frame_root.gd"


var _camera: RefFrameNode = null
var _sun_position: RefFrameNode = null

var _rf: RefFrameNode = null

func _init():
	pass


# Called when the node enters the scene tree for the first time.
func _ready():
	_sun_position = get_node( "SunPosition" )
	_camera = get_node( "SunPosition/Planet/Rotation/RefFrameNode/Camera" )
	_camera.map_mode = false
	var atm: Node = _camera.get_node( "Camera/Atmosphere" )
	atm.visible = false

	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 15000000.0 )
	_camera.transform = t
	
	
	var planet: Node = get_node( "SunPosition/Planet" )




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _ign_pre_process( _delta ):
	#UserInput.gui_control_bool( "ui_map", true, false, false )
	pass





