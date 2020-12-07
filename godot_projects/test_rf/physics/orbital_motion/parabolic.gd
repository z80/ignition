
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )


# Initialization doe not initialize time to periapsis.
# So it is not complete.

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	# Semi-latus-rectum in parabolic case.
	var abs_h: float = args.h.length()
	var slr: float = (abs_h * abs_h) / args.gm
	# Filling in parameters
	args.slr = slr
	
	# True anomaly and distance connection formula.
	# r = slr/(1+cos(true_anomaly))
	# Determine the sign of the "true_anomaly" from dot(v, e).
	var abs_r: float = r.length()
	var true_anomaly: float = acos(slr/abs_r - 1.0)
	var neg: bool = ( args.e.dot( v ) < 0.0 )
	if neg:
		true_anomaly = -true_anomaly
	
	var D: float = tan( true_anomaly * 0.5 )
	var t: float = 0.5 * sqrt( (slr*slr*slr)/args.gm ) * ( D + (D*D*D)/3.0 )
	if neg: 
		t = -t
	
	args.periapsis_t = t


static func process( args: Dictionary, dt: float ):
	var periapsis_t: float = args.periapsis_t + dt
	args.periapsis_t = periapsis_t
	
	var slr: float = args.slr
	
	var rp = slr * 0.5
	var A: float = 1.5 * sqrt( args.gm/(2.0*rp*rp*rp) ) * periapsis_t
	var B: float = pow(A + sqrt(A*A+1.0), 1.0/3.0)
	var true_anomaly: float = 2.0*atan( B - 1.0/B )
	
	var co_f: float = cos( true_anomaly )
	var si_f: float = cos( true_anomaly )
	
	var r: float = slr/(1.0 + co_f)
	
	var r_x: float = r * co_f
	var r_y: float = r * si_f
	var r2: Vector2 = Vector2( r_x, r_y )
	
	var v2: Vector2 = Velocity.velocity( args, true_anomaly )
	



