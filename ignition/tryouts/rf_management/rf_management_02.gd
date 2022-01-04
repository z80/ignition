extends Node


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _enter_tree():
	# In order to assign in before everything else.
	BodyCreator.root_node = self

# Called when the node enters the scene tree for the first time.
func _ready():
	
	DDD.print( "Hello!" )
	
	var camera: RefFrameNode = get_node( "Camera" )
	camera.set_mode( PlayerCamera.Mode.TPS_AZIMUTH )
	DDD.print( "Setup camera!" )
#	var cp = camera.get_parent()
#	if cp != null:
#		cp.remove_child( camera )
#	RootScene.get_root_for_visuals().add_child( camera )
	
	var character = _create_character( "my_character", Vector3( 1.0, 0.0, 0.0 ), 1.0 )
	DDD.print( "Created a character!" )
	
	# Initial set of select and focus object.
	PhysicsManager.camera           = camera
	PhysicsManager.player_control   = character
	PhysicsManager.player_select    = character
	
	camera.apply_target()
	
	#var character2 = _create_character( "my_character", Vector3( 1.0, 0.0, -0.02 ), 1.0 )
	#_create_other_body()
	var construction = _create_construction()
	DDD.print( "Created a construction!", 1.0, "important" )
	
	var se3: Se3Ref = character.relative_to( construction )
	DDD.print( "And this se3 is computed for debug purposes!" )
	
	UiSound.play( "res://physics/singletons/sound/assets/music_01.ogg" )

	
	
	
	
	
func _create_other_body():
	var celestial_body: RefFrameNode = get_node( "Sun/Planet" ) as RefFrameNode
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.2, -0.2 ), 2.0 )

	var body: Body = BodyCreator.create( "cube" )
	body.name = "MyCylinder"
	
	var rf: RefFrameNode = PhysicsManager.create_ref_frame_physics()
	rf.name = "OtherRefFramePhysics"
	var rot = celestial_body.rotation_rf()
	
	rf.change_parent( rot )
	rf.set_se3( se3 )

	body.change_parent( rf )



func _create_construction():
	# Add construction to null ref. frame. 
	# It is suppsed to be picked up when close enough.
	var construction: Body = BodyCreator.create( "construction" )
	
	var celestial_body: RefFrameNode = get_node( "Sun/Home" ) as RefFrameNode
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( Vector3( 1.0, 0.0, -0.00002 ), 1.0 )
	
	var rot = celestial_body.rotation_rf()
	construction.change_parent( rot )
	construction.set_se3( se3 )
	
	return construction



func _create_character( name: String = "MyCharacter", at: Vector3 = Vector3(1.0, 0.0, 0.0), height: float = 1.0 ):
	var rf = PhysicsManager.create_ref_frame_physics()
	rf.name = "rf_p for " + name
	var celestial_body = get_node( "Sun/Home" ) as RefFrameNode
	var rot = celestial_body.rotation_rf()
	rf.change_parent( rot )
	
	var character: Body = BodyCreator.create( "character_central" )
	character.name = name
	# Call add body to process inclusion and initialization properly.
	character.change_parent( rf )
	
	var surf: CubeSphereNode = celestial_body.surface_node()
	var se3: Se3Ref = surf.surface_se3( at, height )
	rf.set_se3( se3 )
	
	se3.transform = Transform.IDENTITY
	se3.v         = Vector3.ZERO
	se3.w         = Vector3.ZERO
	character.set_se3( se3 )
	
	return character
