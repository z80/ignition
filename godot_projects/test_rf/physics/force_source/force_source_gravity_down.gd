
extends ForceSource
class_name ForceSourceGravityDown

export(float) var GM = 1.0


func compute_force( body: Body, r: Vector3, v: Vector3, q: Quat, w: Vector3, ret: Array ):
	var F: Vector3 = Vector3( 0.0, -1.0, 0.0 )
	F = q.xform( F )
	var P: Vector3 = Vector3.ZERO
	
	ret.clear()
	ret.push_back( F )
	ret.push_back( P )


func up( own_rf: Node, player_rf: Node ):
	var Utils = preload( "res://physics/utils/local_up.gd" )
	var up: Vector3 = Vector3.UP
	up =  Utils.local_up( own_rf, up, player_rf )
	return up




