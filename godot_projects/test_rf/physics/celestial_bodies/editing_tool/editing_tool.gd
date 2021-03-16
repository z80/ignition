
extends Node


var _camera: Camera = null

var _rf: RefFrame = null

func _init():
	# Make creator know where to add visual and physical objects.
	BodyCreator.root_node = self

# Called when the node enters the scene tree for the first time.
func _ready():
	_camera = get_node( "Camera" )
	var sun = get_node( "Sun" )
	sun.convert_to_global = true
	sun.apply_scale = false
	_rf = RefFrame.new()
	
	_rf.change_parent( get_node( "Node" ) )
	
	PhysicsManager.camera = _camera
	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 50.0 )
	_camera.transform = t
	
	#sun.set_origin( rf )




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var t: Transform = _camera.transform
	_rf.set_t( t )
	
	var sun = get_node( "Sun" )
	sun.process( delta, _rf )
