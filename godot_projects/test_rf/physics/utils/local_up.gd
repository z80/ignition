
# This is auxilary function
static func local_up( own_rf_node: Node, up: Vector3, player_rf_node: Node ):
	var own_rf: RefFrame    = own_rf_node as RefFrame
	var player_rf: RefFrame = player_rf_node as RefFrame
	own_rf.compute_relative_to_root( player_rf )
	var q: Quat = own_rf.q_root()
	up = q.xform( up )
	return up
