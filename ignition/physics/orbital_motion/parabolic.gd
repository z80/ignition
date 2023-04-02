
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )


# Initialization doe not initialize time to periapsis.
# So it is not complete.

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	var inv_A: Basis = args.inv_A
	r = inv_A * (r)
	v = inv_A * (v)
	
	# Semi-latus-rectum in parabolic case.
	var abs_h: float = args.h.length()
	var slr: float = (abs_h * abs_h) / args.gm
	
	# True anomaly and distance connection formula.
	# r = slr/(1+cos(true_anomaly))
	# Determine the sign of the "true_anomaly" from dot(v, e).
	var abs_r: float = r.length()
	# Due to numerical precision need to do this.
	var arg: float = slr/abs_r - 1.0
	if arg > 1.0:
		arg = 1.0
	elif arg < -1.0:
		arg = -1.0
	var true_anomaly: float = acos( arg )
	var neg: bool = ( r.y < 0.0 )
	if neg:
		true_anomaly = -true_anomaly
	
	var D: float = tan( true_anomaly * 0.5 )
	var t: float = 0.5 * sqrt( (slr*slr*slr)/args.gm ) * ( D + (D*D*D)/3.0 )
	
	# Filling in parameters
	args.slr = slr
	args.periapsis_t = t


static func process( dt: float, args: Dictionary ):
	var periapsis_t: float = args.periapsis_t + dt
	args.periapsis_t = periapsis_t
	
	var slr: float = args.slr
	
	var rp = slr * 0.5
	var A: float = 1.5 * sqrt( args.gm/(2.0*rp*rp*rp) ) * periapsis_t
	var B: float = pow(A + sqrt(A*A+1.0), 1.0/3.0)
	var true_anomaly: float = 2.0*atan( B - 1.0/B )
	
	var co_f: float = cos( true_anomaly )
	var si_f: float = sin( true_anomaly )
	
	var abs_r: float = slr/(1.0 + co_f)
	
	var r_x: float = abs_r * co_f
	var r_y: float = abs_r * si_f
	var r: Vector3 = Vector3( r_x, r_y, 0.0 )
	
	var v: Vector3 = velocity( args, r )
	
	var Aw: Basis = args.A
	r = Aw * (r)
	v = Aw * (v)
	
	return [r, v]






static func velocity( args: Dictionary, r: Vector3 ):
	var gm: float    = args.gm
	var a: float     = args.a
	var abs_e: float = args.abs_e
	var abs_r: float = r.length()
	
	var si_f: float = r.y/abs_r
	var co_f: float = r.x/abs_r
	
	var v: Vector3 = Vector3( -si_f, co_f + 1.0, 0.0 )
	v = v.normalized()
	var abs_v: float = sqrt( (2.0*gm)/abs_r )
	v *= abs_v
	return v

