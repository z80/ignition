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
	var celestial_body = get_node( "CelestialBody" ) as CelestialBody
	var rot = celestial_body.rotation_rf()
	var cp: bool = rf.has_method( "change_parent" )
	rf.change_parent( rot )
	
	var capsule: Body = BodyCreator.create( "character_simple" )
	# Call add body to process inclusion and initialization properly.
	rf.add_body( capsule )
	
	# Add construction to null ref. frame. 
	# It is suppsed to be picked up when close enough.
	#var construction: Body = BodyCreator.create( "construction" )
	
	var camera: Camera = get_node( "Camera" )
	camera.set_mode( PlayerCamera.Mode.TPS_AZIMUTH )
	
	# Initial set of select and focus object.
	PhysicsManager.player_ref_frame = rf
	PhysicsManager.camera           = camera
	PhysicsManager.player_control   = capsule
	PhysicsManager.player_select    = capsule


