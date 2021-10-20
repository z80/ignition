
extends ForceSource
class_name ForceSourceGravity

export(float) var GM = 1.0


func compute_force( body: Body, se3: Se3Ref ):
	# Later need to add body's mass to the formula.
	# For now it comuted free fall acceleration.
	var r: Vector3 = se3.r
	var abs_r: float = r.length()
	var dont_compute: bool = ( abs_r < 0.001 )
	
	if dont_compute:
		var ret: Array = []
		ret.push_back( Vector3.ZERO )
		ret.push_back( Vector3.ZERO )
		return ret
	
	var m: float = body.get_mass()
	var F: Vector3 = ( (GM*m)/(abs_r*abs_r*abs_r) ) * r
	var P: Vector3 = Vector3.ZERO
	
	var ret: Array = []
	ret.push_back( F )
	ret.push_back( P )
	return ret











