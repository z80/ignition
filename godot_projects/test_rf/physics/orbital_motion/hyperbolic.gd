
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	# Semi-latus-rectum.
	var slr: float = (args.abs_h*args.abs_h) / args.gm
	
	var abs_r: float = r.length()
	# True anomaly.
	# r = l / ( 1.0 + e*cos(f) )
	# Solve for "f" and define the sign using dot product of (r,v)
	var abs_r: float = r.length()
	var f: float = acos( (slr/abs_r - 1.0) / args.abs_e )
	var r_dot_v: float = r.dot( v )
	var negative: bool = (r_dot_v < 0.0)
	if negative:
		f = -f

	# Eccentric anomaly.
	# cosh(E) = (cos(f) + e) / (1.0 + e*cos(f))
	var co_f = cos(f)
	var E: float = acosh( (co_f + args.abs_e) / (1.0 + args.abs_e*co_f) )
	if negative:
		E = -E

	# Periapsis crossing time
	var a: float  = args.a
	var gm: float = args.gm
	var n: float  = sqrt( -(a*a*a)/gm )
	var periapsis_t: float = (args.abs_e * sinh(E) - E) * n
	if negative:
		periapsis_t = -periapsis_t
	
	# Filling in parameters
	args.slr = slr
	args.E   = E
	args.n   = n
	args.periapsis_t = periapsis_t


static func process( args: Dictionary, dt: float ):
	var periapsis_t: float = args.periapsis_t + dt
	args.periapsis_t = periapsis_t
	var a: float = args.a
	var gm: float = args.gm
	var e: float = args.abs_e
	var n: float = args.n
	var slr: float = args.slr

	var M: float = periapsis_t / n
	var E: float = solve( e, M, args.E )
	
	var co_E: float = cosh( E )
	# cosh(E) = (cos(f) + e) / (1.0 + e*cos(f))
	const f: float = acos( (e - co_E) / (e*co_E - 1.0) )
	if E < 0.0:
		f = -f
	
	# r = slr/(1.0 + e*cos(f))
	var si_f = sin(f)
	var co_f = cos(f)
	var r: float = slr / ( 1.0 + e*co_f )
	var x: float = r * co_f
	var y: float = r * si_f
	var r2: Vector2 = Vector2( x, y )

	var v2: Vector2 = Velocity.velocity( args, f )

	return [ r2, v2 ]


static func solve( e: float, M: float, E: float ):
	var exp_E: float = exp(E)
	var ret: Array = solve_next( e, M, E, exp_E )
	var err: float = ret[0]
	var En: float = ret[1]
	var exp_En: float  = ret[2]
	var iters: int = 0
	
	while ( err > Consts.EPS ) and ( iters < Consts.MAX_ITERS ):
		ret = solve_next( e, M, En, exp_En )
		err = ret[0]
		En = ret[1]
		exp_En  = ret[2]
	return En
	


static func solve_next( e: float, M: float, E: float, exp_E: float ):

	var next_exp_E: float = ( 2.0*exp_E*(e-exp_E+exp_E*E+exp_E*M) ) / ( e*exp_E*exp_E-2.0*exp_E + e )
	var next_E: float = log(next_exp_E)
	var err: float = abs( 0.5*e*(next_exp_E - 1.0/next_exp_E) - next_E - M )
	return [err, next_E, next_exp_E]

