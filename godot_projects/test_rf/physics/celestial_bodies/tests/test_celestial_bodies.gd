
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
	rf.add_child( camera )
	
	PhysicsManager.player_ref_frame = rf
	PhysicsManager.camera = camera
	
	rf.change_parent( sun.rotation_rf() )
	
	rf.init_physics()
	
	var t: Transform = Transform.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 10.0 )
	rf.transform = t
	
	sun.set_origin( rf )




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var sun = get_node( "Sun" )
	sun.process( delta )
