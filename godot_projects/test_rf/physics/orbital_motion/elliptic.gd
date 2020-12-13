
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	r = args.inv_A.xform( r )
	v = args.inv_A.xform( v )
	
	var a: float     = args.a
	var gm: float    = args.gm
	var abs_e: float = args.abs_e
	
	# Semi-latus-rectum.
	var slr: float = args.a * (1.0 - abs_e*abs_e)
	
	var b: float = a * sqrt(1.0 - abs_e*abs_e)
	var cos_E: float = (r.x + a*abs_e) / a
	var sin_E: float = r.y / b

	var E: float = atan2( sin_E, cos_E )
	
	var n: float = sqrt( args.a*args.a*args.a/gm )
	# Orbital period.
	var T: float = 2.0*PI*n
	# Time since periapsis.
	var periapsis_t: float = n * ( E - abs_e*sin(E) )
	
	# Filling in parameters
	args.b   = b
	args.slr = slr
	args.n   = n
	args.E   = E
	args.T   = T
	args.periapsis_t = periapsis_t


static func process( dt: float, args: Dictionary ):
	var T: float = args.T
	var periapsis_t: float = args.periapsis_t + dt
	if periapsis_t > T:
		periapsis_t -= T
	args.periapsis_t = periapsis_t
	
	var a: float     = args.a
	var b: float     = args.b
	var abs_e: float = args.abs_e
	
	var M: float = periapsis_t / args.n
	var E: float = solve( abs_e, M, args.E )
	
	var co_E: float = cos( E )
	var si_E: float = sin( E )
	var x: float = a*(co_E - abs_e)
	var y: float = b*si_E
	var r: Vector3 = Vector3( x, y, 0.0 )
	print( "local r: ", r )

	var v: Vector3 = velocity( args, r, E )
	print( "local v: ", v )
	
	var A: Basis = args.A
	r = A.xform( r )
	v = A.xform( v )
	
	return [r, v]


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



static func velocity( args: Dictionary, r: Vector3, E: float ):
	var gm: float = args.gm
	var a: float = args.a
	var b: float = args.b
	var abs_r: float = r.length()
	var v: Vector3 = Vector3( -a*sin(E), b*cos(E), 0.0 )
	v = v.normalized()
	var abs_v: float = sqrt( gm*( 2.0/abs_r - 1.0/a ) )
	v *= abs_v
	return v


