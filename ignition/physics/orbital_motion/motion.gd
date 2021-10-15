
const Consts   = preload( "res://physics/orbital_motion/constants.gd" )
const Elliptic   = preload( "res://physics/orbital_motion/elliptic.gd" )
const Hyperbolic = preload( "res://physics/orbital_motion/hyperbolic.gd" )
const Parabolic  = preload( "res://physics/orbital_motion/parabolic.gd" )
const Linear     = preload( "res://physics/orbital_motion/linear.gd" )

enum { LINEAR, ELLIPTIC, PARABOLIC, HYPERBOLIC }

static func init( gm: float, r: Vector3, v: Vector3 ):
	# Specific angular momentum.
	var h: Vector3 = r.cross( v )
	var abs_h: float = h.length()
	
	# Equations don't work for free fall condition.
	if abs_h < Consts.MIN_ANGULAR_MOMENTUM:
		var args = {
			type = LINEAR, 
			gm = gm
		}
		Linear.init( r, v, args )
		return false
	
	var abs_r: float = r.length()
	
	var ea: Vector3 = v.cross( h ) / gm
	var eb: Vector3 = r / abs_r
	# Eccentricity vector.
	var e: Vector3 = ea - eb
	var abs_e: float = e.length()
	
	var abs_v: float = v.length()
	var Ws: float = gm/abs_r - 0.5*abs_v*abs_v
	# Semimajor axis.
	var a: float
	if ( abs_e > (1.0 + Consts.EPS) ) or ( abs_e < ( 1.0 - Consts.EPS ) ):
		a = 0.5*gm/Ws
	else:
		a = 0.0
	
	# Unit vectors.
	# "e_a" towards perigee.
	# "e_b" 90 deg to "e_a" in the orbit plain and towards object movement.
	var e_x: Vector3
	if abs_e > Consts.EPS:
		e_x = e / abs_e
	else:
		e_x = r.normalized()
	
	var h_cross_ex: Vector3 = h.cross( e_x )
	var abs_h_corss_ex: float = h_cross_ex.length()
	var e_y: Vector3
	if abs_h_corss_ex > Consts.EPS:
		e_y = h_cross_ex / abs_h_corss_ex
	else:
		e_y = v.normalized()
		e_y = e_y - e_y.project( e_x )
		e_y = e_y.normalized()
	
	var e_z: Vector3 = e_x.cross( e_y )
	var A: Basis
	A.x = e_x
	A.y = e_y
	A.z = e_z
	var inv_A: Basis = A.inverse()
	
	var args = {
		gm = gm, 
		h  = h, 
		e = e, 
		abs_e = abs_e, 
		a = a, 
		A = A, 
		inv_A = inv_A
	}
	
	if abs_e > (1.0 + Consts.EPS):
		args.type = HYPERBOLIC
		Hyperbolic.init( r, v, args )
	elif ( abs_e < ( 1.0 - Consts.EPS ) ):
		args.type = ELLIPTIC
		Elliptic.init( r, v, args )
	else:
		args.type = PARABOLIC
		Parabolic.init( r, v, args )
	
	return args



static func process( dt: float, args: Dictionary ):
	var t: int = args.type
	
	var ret: Array
	if t == LINEAR:
		ret = Linear.process( dt, args )
	if t == HYPERBOLIC:
		ret = Hyperbolic.process( dt, args )
	elif t == ELLIPTIC:
		ret = Elliptic.process( dt, args )
	elif t == PARABOLIC:
		ret = Parabolic.process( dt, args )
	else:
		ret = [ Vector3.ZERO, Vector3.ZERO ]
	
	return ret



# On demand launching bodies.
# "unit_r"  direction towards perigee.
# "unit_v"  velocity direction at perigee.
# "ecc"     desired eccentricity.
# "T"       desired period.
static func launch_elliptic( gm: float, unit_r: Vector3, unit_v: Vector3, T: float, ecc: float ):
	var ee: float = (1.0+ecc)/(1.0-ecc)
	var pi_T: float = 2.0*PI*gm/T
	var arg: float = pi_T*pi_T*ee*ee*ee
	var v: float = pow(arg, 1.0/6.0)
	var r: float = (1.0+ecc)*gm/(v*v)
	
	var v_r: Vector3 = unit_r.normalized() * r
	var v_v: Vector3 = unit_v.normalized() * v
	
	return [v_r, v_v]


static func init_elliptic( gm: float, unit_r: Vector3, unit_v: Vector3, T: float, ecc: float ):
	var ret: Array = launch_elliptic( gm, unit_r, unit_v, T, ecc )
	var st: Dictionary = init( gm, ret[0], ret[1] )
	return st


# Compute "GM" based on surface radius and circular velocity desired
static func init_gm( r: float, v: float ):
	return r*v*v

