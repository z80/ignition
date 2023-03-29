
extends "res://physics/ref_frames/root/ref_frame_root.gd"


var _camera: RefFrameNode = null
var _sun_position: RefFrameNode = null

var _rf: RefFrameNode = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_sun_position = get_node( "SunPosition" )
	_camera = get_node( "SunPosition/Planet/Camera" )
	_camera.map_mode = true
	var atm: Node = _camera.get_node( "Camera/Atmosphere" )
	atm.visible = false

	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 15000000.0 )
	_camera.transform = t
	
	
	var planet: Node = get_node( "SunPosition/Planet" )
	var surfaces: Node = planet.surfaces_node()
	var ui: Node = get_node( "Ui" )
	ui.surfaces = surfaces




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _ign_pre_process( _delta ):
#	var t: Transform = _camera.transform
#	_rf.set_t( t )
#
#	var se3: Se3Ref = _sun_position.relative_to( _camera )
#
#	var planet: RefFrameNode = get_node( "Planet" )
#	planet.process( _delta, null )
	
	UserInput.gui_control_bool( "ui_map", true, false, false )
	
#	_camera.apply_atmosphere( planet )
#	_camera.place_light( _sun_position )
#	_camera.process( _delta )





