extends Node


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _enter_tree():
	# In order to assign in before everything else.
	BodyCreator.root_node = self

# Called when the node enters the scene tree for the first time.
func _ready():
	var camera: Camera = get_node( "Camera" )
	camera.set_mode( PlayerCamera.Mode.TPS_AZIMUTH )
	
	var character = _create_character( "my_character", Vector3( 1.0, 0.0, 0.0 ), 1.0 )
	
	# Initial set of select and focus object.
	PhysicsManager.camera           = camera
	PhysicsManager.player_control   = character
	PhysicsManager.player_select    = character
	
	camera.apply_target()
	
	var character2 = _create_character( "my_character", Vector3( 1.0, 0.0, -0.02 ), 1.0 )
	
	
	#_create_other_body()
	_create_construction()
	
	
	
func _create_other_body():
	var celestial_body: CelestialBody = get_node( "Sun/CelestialBody" ) as CelestialBody
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.2, -0.2 ), 2.0 )

	var body: Body = BodyCreator.create( "cube" )
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



func _create_construction():
	# Add construction to null ref. frame. 
	# It is suppsed to be picked up when close enough.
	var construction: Body = BodyCreator.create( "construction" )
	
	var celestial_body: CelestialBody = get_node( "Sun/CelestialBody" ) as CelestialBody
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.0, -0.02 ), 1.0 )
	
	var rot = celestial_body.rotation_rf()
	construction.change_parent( rot )
	construction.set_se3( se3 )



func _create_character( name: String = "MyCharacter", at: Vector3 = Vector3(1.0, 0.0, 0.0), height: float = 1.0 ):
	var rf = PhysicsManager.create_ref_frame_physics()
	rf.name = "rf_p for " + name
	var celestial_body = get_node( "Sun/CelestialBody" ) as CelestialBody
	var rot = celestial_body.rotation_rf()
	rf.change_parent( rot )
	
	var character: Body = BodyCreator.create( "character_central" )
	character.name = name
	# Call add body to process inclusion and initialization properly.
	character.change_parent( rf )
	
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( at, height )
	rf.set_se3( se3 )
	
	return character
