
extends ForceSource
class_name ForceSourceRotational

export(Vector3) var ang_velocity = Vector3.ZERO

func compute_force( body: Body, se3: Se3Ref ):
	var m: float = body.get_mass()
	var inv_q: Quat = se3.q.inverse()
	var r: Vector3  = se3.r
	var w: Vector3  = inv_q.xform( ang_velocity )
	
	var centrifugal: Vector3 = -m * w.cross( r )
	var coriolis: Vector3    = -2.0*m*( w.cross( w.cross( r ) ) )
	var euler: Vector3       = Vector3.ZERO
	
	var F: Vector3 = centrifugal + coriolis + euler
	var P: Vector3 = Vector3.ZERO
	
	var ret: Array = []
	ret.push_back( F )
	ret.push_back( P )
	return ret











