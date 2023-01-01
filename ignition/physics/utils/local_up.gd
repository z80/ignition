
# This is auxilary function
static func local_up( own_rf_node: Node, up: Vector3, player_rf_node: Node ):
	var own_rf: RefFrame    = own_rf_node as RefFrame
	var player_rf: RefFrame = player_rf_node as RefFrame
	var se3: Se3Ref = own_rf.relative_to( player_rf )
	var q: Quat = se3.q
	up = q.xform( up )
	return up
