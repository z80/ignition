
extends Node


var camera: Camera3D = null
var rf_physics: RefFramePhysics = null

func _init():
	# Make creator know where to add visual and physical objects.
	BodyCreator.root_node = self


# Called when the node enters the scene tree for the first time.
func _ready():
	var sun = get_node( "Sun" )
	
	var rf = RefFramePhysics.new()
	rf.name = "my_physics_rf"
	camera = Camera3D.new()
	var camera_script = preload( "res://assets/maujoe.camera_control/scripts/camera_control.gd" )
	camera.script = camera_script
	
	rf.change_parent( sun )
	# Parent camera to the root node of this scene.
	RootScene.get_visual_layer_near().add_child( camera )
	
	var player_control: RefFrame = RefFrame.new()
	rf.init_physics()
	player_control.change_parent( rf )
	RootScene.ref_frame_root.player_control = player_control
	
	#RootScene.ref_frame_root.player_camera = camera
	
	
	var t: Transform3D = Transform3D.IDENTITY
	t.origin = Vector3( 0.0, 0.0, 10.0 )
	camera.transform = t
	
	rf_physics = rf





# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var sun = get_node( "Sun" )
	sun.process( delta )
	
	if rf_physics != null:
		var t: Transform3D = camera.transform
		rf_physics.transform.origin = t.origin
