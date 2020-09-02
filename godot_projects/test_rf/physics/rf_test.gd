
extends Node




# Called when the node enters the scene tree for the first time.
func _ready():
	var qa: Quat = Quat( 0.0, 0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0) )
	var qb: Quat = Quat( 0.0, 1.0, 0.0, 0.0 )
	var q = qa * qb
	print( "IMU q: ", q )

	BodyCreator.root_node = self
	PhysicsManager.player_ref_frame = $Landscape/Rf
	PhysicsManager.camera = $Camera
	
	
	#var rf = $Rf
	#rf.create_body( "plane" )
	#rf.create_body( "cube" )
	
	var rf = $Landscape/Rf
	rf.init_physics()
	
	var capsule: Body = BodyCreator.create( "capsule_dbg" )
	# Call add body to process inclusion and initialization properly.
	rf.add_body( capsule )
	
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

	
	var privot: Spatial = capsule._visual
	$Camera.privot = privot
	
	# Initial set of select and focus object.
	PhysicsManager.player_select = capsule
	PhysicsManager.player_focus  = capsule



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	#var group: String = Body.GROUP_NAME
	#var player_rf = PhysicsManager.player_ref_frame
	#for body in get_tree().get_nodes_in_group( group ):
	#	body.update_visual( player_rf )
	pass
