extends Node


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _enter_tree():
	# In order to assign in before everything else.
	BodyCreator.root_node = self

# Called when the node enters the scene tree for the first time.
func _ready():
	var rf = PhysicsManager.create_ref_frame_physics()
	rf.name = "MyRefFramePhysics"
	var celestial_body = get_node( "CelestialBody" ) as CelestialBody
	var rot = celestial_body.rotation_rf()
	rf.change_parent( rot )
	
	var character: Body = BodyCreator.create( "character_central" )
	character.name = "MyCharacter"
	# Call add body to process inclusion and initialization properly.
	character.change_parent( rf )
	
	# Add construction to null ref. frame. 
	# It is suppsed to be picked up when close enough.
	#var construction: Body = BodyCreator.create( "construction" )
	
	var camera: Camera = get_node( "Camera" )
	camera.set_mode( PlayerCamera.Mode.TPS_AZIMUTH )
	
	# Initial set of select and focus object.
	PhysicsManager.player_ref_frame = rf
	PhysicsManager.camera           = camera
	PhysicsManager.player_control   = character
	PhysicsManager.player_select    = character
	
	camera.apply_target()

	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.0, 0.0 ), 2.0 )
	rf.set_se3( se3 )
	
	_create_other_body()
	
	
	
func _create_other_body():
	var celestial_body: CelestialBody = get_node( "CelestialBody" ) as CelestialBody
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.2, -0.2 ), 2.0 )

	var body: Body = BodyCreator.create( "part_cylinder" )
	body.name = "MyCylinder"
	
	# Call add body to process inclusion and initialization properly.
	#var rf: RefFrameNode = celestial_body.rotation_rf()
	#body.change_parent( rf )
	#body.set_se3( se3 )
	
	var rf = PhysicsManager.create_ref_frame_physics()
	rf.name = "OtherRefFramePhysics"
	body.change_parent( rf )
	var rot = celestial_body.rotation_rf()
	rf.change_parent( rot )
	rf.set_se3( se3 )

	



