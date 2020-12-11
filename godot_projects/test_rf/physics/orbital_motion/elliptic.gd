
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	var gm: float    = args.gm
	var abs_e: float = args.abs_e
	# Semi-latus-rectum.
	var slr: float = args.a * (1.0 - args.abs_e*args.abs_e)
	
	var abs_r: float = r.length()
	var cos_E: float = 1.0 - abs_r/args.a
	var r_dot_v: float = r.dot( v )
	var sin_E: float = r_dot_v / ( abs_e * sqrt( gm * args.a ) )
	var E: float = atan2( sin_E, cos_E )
	
	var n: float = sqrt( args.a*args.a*args.a/gm )
	# Orbital period.
	var T: float = 2.0*PI*n
	# Time to periapsis.
	var periapsis_t: float = ( abs_e*sin(E) - E ) * n
	
	# Filling in parameters
	args.slr = slr
	args.n   = n
	args.E   = E
	args.T   = T
	args.periapsis_t = periapsis_t


static func process( dt: float, args: Dictionary ):
	var periapsis_t: float = args.periapsis_t + dt
	if periapsis_t > args.T:
		periapsis_t -= args.T
	args.periapsis_t = periapsis_t
	
	var M: float = periapsis_t / args.n
	var E: float = solve( args.abs_e, M, args.E )
	
	var co_E: float = cos( E )
	var si_E: float = sin( E )
	var x: float = args.a*(co_E - args.abs_e)
	var y: float = args.a*(1.0 - args.abs_e*args.abs_e)*si_E
	var r2: Vector2 = Vector2( x, y )

	var f: float = abs( acos( (args.abs_e - co_E)/(args.abs_e*co_E - 1.0) ) )
	var v2: Vector2 = Velocity.velocity( args, f )
	
	var r3: Vector3 = (args.e_x * r2.x) + (args.e_y * r2.y)
	var v3: Vector3 = (args.e_x * v2.x) + (args.e_y * v2.y)
	
	return [r3, v3]


static func solve( e: float, M: float, E: float ):
	var ret: Array = solve_next( e, M, E )
	var err: float = ret[0]
	var En: float  = ret[1]
	var iters: int = 0
	
	while ( err > Consts.EPS ) and ( iters < Consts.MAX_ITERS ):
		ret = solve_next( e, M, En )
		err = ret[0]
		En  = ret[1]
		
		iters += 1
		
	if En > PI:
		En -= Consts._2PI
	elif En < -PI:
		En += Consts._2PI
		
	return En
	


static func solve_next( e: float, M: float, E: float ):
	var si_E: float = sin( E )
	var co_E: float = cos( E )
	var En: float = E - ( E - e*si_E - M ) / (1.0  - e*co_E)
	var si_En: float = sin( En )
	var err: float = abs( En - e*si_En - M )
	return [err, En]

