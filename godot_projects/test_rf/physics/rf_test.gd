
extends Node




# Called when the node enters the scene tree for the first time.
func _ready():
	var qa: Quat = Quat( 0.0, 0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0) )
	var qb: Quat = Quat( 0.0, 1.0, 0.0, 0.0 )
	var q = qa * qb
	print( "IMU q: ", q )

	BodyCreator.root_node = self
	PhysicsManager.player_ref_frame = $Landscape/Rf
	
	
	#var rf = $Rf
	#rf.create_body( "plane" )
	#rf.create_body( "cube" )
	#rf.init_physics()
	
	var rf = $Landscape/Rf
	var capsule: Body = BodyCreator.create( "capsule_dbg" )
	capsule.change_parent( rf )
	
	var privot: Spatial = $Landscape/Rf/CapsuleDbg._visual
	$Camera.privot = privot



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	#var group: String = Body.GROUP_NAME
	#var player_rf = PhysicsManager.player_ref_frame
	#for body in get_tree().get_nodes_in_group( group ):
	#	body.update_visual( player_rf )
	pass
