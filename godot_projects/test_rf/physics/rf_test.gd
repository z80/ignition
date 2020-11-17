
extends Node



func test_quats():
	var q1: Quat = Quat( 0.0, 0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0) )
	var qb: Quat = Quat( 1.0, 0.0, 0.0, 0.0 )
	var q = q1 * qb * q1.inverse()
	print( "expect to get (0, 1, 0, 0) q: ", q )
	
	
	
	
	var axis: Vector3 = Vector3( 0, 1, 0 )
	
	var az: float = atan2( axis.y, axis.x )
	var el: float = asin( axis.z )
	
	var q_90: Quat = Quat( Vector3(0, 1, 0), PI/2.0 )
	
	var q_az = Quat( Vector3( 0, 0, 1 ), az )
	var q_el = Quat( Vector3( 1, 0, 0 ), el )
	
	var q_all: Quat = q_az * q_el * q_90
	var qq = q_az * q_el
	
	var t: Quat = Quat( 0, 0, 1, 0 )
	t = q_all * t * q_all.inverse()
	
	var ea: Vector3 = q_all.get_euler()
	
	var q_from_ea_x: Quat = Quat( Vector3(0.0, ea.x, 0.0) )
	var q_from_ea_y: Quat = Quat( Vector3(ea.y, 0.0, 0.0) )
	var q_from_ea_all: Quat = q_from_ea_x * q_from_ea_y
	
	var q_from_ea: Quat = Quat( ea )
	
	print( "t: ", t )
	
	
	


# Called when the node enters the scene tree for the first time.
func _ready():
	test_quats()
	
	BodyCreator.root_node = self
	PhysicsManager.player_ref_frame = $Landscape/Rf
	PhysicsManager.camera = $Camera
	
	
	#var rf = $Rf
	#rf.create_body( "plane" )
	#rf.create_body( "cube" )
	
	var rf = $Landscape/Rf
	rf.init_physics()
	
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
