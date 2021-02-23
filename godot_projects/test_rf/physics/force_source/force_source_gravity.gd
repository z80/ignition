
extends ForceSource
class_name ForceSourceGravity

export(float) var GM = 1.0


func compute_force( body: Body, r: Vector3, v: Vector3, q: Quat, w: Vector3, ret: Array ):
	# Later need to add body's mass to the formula.
	# For now it comuted free fall acceleration.
	var abs_r: float = r.length()
	if ( abs_r < 0.001 ):
		ret.clear()
		ret.push_back( Vector3.ZERO )
		ret.push_back( Vector3.ZERO )
		return
	
	var F: Vector3 = ( GM/(abs_r*abs_r*abs_r) ) * r
	var P: Vector3 = Vector3.ZERO
	
	ret.clear()
	ret.push_back( F )
	ret.push_back( P )


# I don't remember what it should return.
func up( force_source_rf: Node, player_rf: Node ):
	var p_rf: RefFrame = player_rf as RefFrame
	var f_rf: RefFrame = force_source_rf as RefFrame
	var se3: Se3Ref = f_rf.relative_to( p_rf )
	var r = se3.r
	var abs_r: float = r.length()
	if abs_r < 0.001:
		return Vector3.UP
	
	r = -(r / abs_r)
	return r








