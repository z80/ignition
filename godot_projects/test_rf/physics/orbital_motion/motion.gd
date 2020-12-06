
const Consts   = preload( "res://physics/orbital_motion/constants.gd" )
var Elliptic   = preload( "res://physics/orbital_motion/elliptic.gd" )
var Hyperbolic = preload( "res://physics/orbital_motion/hyperbolic.gd" )
var Parabolic  = preload( "res://physics/orbital_motion/parabolic.gd" )
var Linear     = preload( "res://physics/orbital_motion/linear.gd" )


static func init_with_state( gm: float, r: Vector3, v: Vector3 ):
	# Specific angular momentum.
	var h: Vector3 = r.cross( v )
	var abs_h: float = h.length()
	
	# Equations don't work for free fall condition.
	if abs_h < Consts.MIN_ANGULAR_MOMENTUM:
		var args = {
			gm = gm
		}
		#Linear.init( r, v, args )
		return false
	
	var abs_r: float = r.length()
	
	var ea: Vector3 = v.cross( h ) / gm
	var eb: Vector3 = r / abs_r
	# Eccentricity vector.
	var e: Vector3 = ea - eb
	
	var abs_v: float = v.length()
	var Ws: float = gm/abs_r - 0.5*abs_v*abs_v
	# Semimajor axis.
	var a: float = 0.5*gm/Ws
	
	# Unit vectors.
	# "e_a" towards perigee.
	# "e_b" 90 deg to "e_a" in the orbit plain and towards object movement.
	var abs_e: float = e.length()
	var e_x: Vector3 = e / abs_e
	
	var h_cross_ex: Vector3 = h.cross( e_x )
	var abs_h_corss_ex: float = h_cross_ex.length()
	var e_y: Vector3 = h_cross_ex / abs_h_corss_ex
	
	var args = {
		gm = gm, 
		r = r, 
		v = v, 
		e = e, 
		abs_e = abs_e, 
		a = a, 
		e_x = e_x, 
		e_y = e_y
	}
	
	if abs_e > (1.0 + Consts.EPS):
		Hyperbolic.init( r, v, args )
	elif ( abs_e < ( 1.0 - Consts.EPS ) ):
		Elliptic.init( r, v, args )
	else:
		Parabolic.init( r, v, args )

