
extends Node


var camera: Camera = null

# Called when the node enters the scene tree for the first time.
func _ready():
	# Make creator know where to add visual and physical objects.
	BodyCreator.root_node = self

	var sun = get_node( "Sun" )
	
	var rf = RefFramePhysics.new()
	camera = Camera.new()
	var camera_script = preload( "res://assets/maujoe.camera_control/scripts/camera_control.gd" )
	camera.script = camera_script
	
	rf.change_parent( sun )
	RootScene.get_root_for_visuals().add_child( camera )
	
	PhysicsManager.camera = camera
	
	rf.init_physics()
	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, -50.0 )
	rf.transform = t
	
	#sun.set_origin( rf )




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var sun = get_node( "Sun" )
	sun.process( delta )
