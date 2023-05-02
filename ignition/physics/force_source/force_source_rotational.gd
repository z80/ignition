
extends ForceSource
class_name ForceSourceRotational

@export var ang_velocity: Vector3 = Vector3.ZERO

func compute_force( body: RefFrameNode, se3: Se3Ref ):
	var m: float    = body.get_mass()
	var inv_q: Quaternion =  se3.q.inverse()
	var r: Vector3  = -se3.r
	var v: Vector3  =  se3.v
	var w: Vector3  =  inv_q * (ang_velocity)
	
	var centrifugal: Vector3 = -( w.cross( w.cross( r ) ) )
	var coriolis: Vector3    = -2.0*w.cross( v )
	# Planet rotates with no angular acceleration.
	# So Euler force is always 0.
	#var euler: Vector3       = Vector3.ZERO
	
	var F: Vector3 = (centrifugal + coriolis) * m
	var P: Vector3 = Vector3.ZERO
	
	var ret: Array = []
	ret.push_back( F )
	ret.push_back( P )
	return ret











