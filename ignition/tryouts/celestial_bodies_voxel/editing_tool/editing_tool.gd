
extends Node


var _camera: RefFrameNode = null
var _sun_position: RefFrameNode = null

var _rf: RefFrameNode = null

func _init():
	# Make creator know where to add visual and physical objects.
	BodyCreator.root_node = self

# Called when the node enters the scene tree for the first time.
func _ready():
	_sun_position = get_node( "SunPosition" )
	_camera = get_node( "Planet/Camera" )
	_camera.map_mode = true
	var atm = _camera.get_node( "Camera/Atmosphere" )
	atm.visible = false

	#var planet = get_node( "Planet" )
	#sun.convert_to_global = false
	#sun.apply_scale = true
	_rf = RefFrame.new()
	
	_rf.change_parent( get_node( "Node" ) )
	
	PhysicsManager.camera = _camera
	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 500.0 )
	_camera.transform = t
	
	#sun.set_origin( rf )




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var t: Transform = _camera.transform
	_rf.set_t( t )
	
	var se3: Se3Ref = _sun_position.relative_to( _camera )
	
	var planet = get_node( "Planet" )
	#planet.process( delta, _rf )
	planet.process( delta, null )
	UserInput.gui_control_bool( "ui_map", true, false, false )
	_camera.apply_atmosphere( planet )
	_camera.place_light( _sun_position )
	_camera.process( delta )
