
extends Node




func _enter_tree():
	# In order to assign in before everything else.
	BodyCreator.root_node = self



# Called when the node enters the scene tree for the first time.
func _ready():
	var rf = PhysicsManager.create_ref_frame_physics()
	rf.change_parent( get_node( "Landscape" ) )
	
	PhysicsManager.player_ref_frame = rf
	PhysicsManager.camera = $Camera
	
	var capsule: Body = BodyCreator.create( "character_simple" )
	# Call add body to process inclusion and initialization properly.
	rf.add_body( capsule )
	PhysicsManager.player_control  = capsule
	PhysicsManager.player_select = capsule
	
		# for debugging purposes
	#var tree = get_tree()
	#var bodies = tree.get_nodes_in_group( Constants.BODIES_GROUP_NAME )
	#print( "bodies: ", bodies )
	
	# Add construction to null ref. frame. 
	# It is suppsed to be picked up when close enough.
	var construction: Body = BodyCreator.create( "construction" )
	
		# for debugging purposes
	#bodies = tree.get_nodes_in_group( Constants.BODIES_GROUP_NAME )
	#print( "bodies: ", bodies )

	
	#var privot: Spatial = capsule._visual
	#$Camera.privot = privot
	$Camera.set_mode( $Camera.Mode.TPS_AZIMUTH )
	
	# Initial set of select and focus object.
	PhysicsManager.player_select  = capsule
	PhysicsManager.player_control = capsule



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	#var group: String = Body.GROUP_NAME
	#var player_rf = PhysicsManager.player_ref_frame
	#for body in get_tree().get_nodes_in_group( group ):
	#	body.update_visual( player_rf )
	pass
