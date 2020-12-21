
const Consts = preload( "res://physics/orbital_motion/constants.gd" )
const Velocity = preload( "res://physics/orbital_motion/velocity.gd" )

static func init( r: Vector3, v: Vector3, args: Dictionary ):
	r = args.inv_A.xform( r )
	v = args.inv_A.xform( v )
	
	var a: float = args.a
	var gm: float = args.gm
	var abs_e: float = args.abs_e
	# Semi-latus-rectum.
	var slr: float = a * (1.0 - abs_e*abs_e)
	var n: float = sqrt( -a*a*a/gm )
	
	# True anomaly from "r".
	#var f: float = atan2( r.y, r.x )
	# Hyperbolic eccentric anomaly E (or H).
	var co_f: float = r.x/r.length()
	var cosh_E: float = (co_f + abs_e)/(1.0 + abs_e*co_f)
	var sinh_E: float = sqrt(cosh_E*cosh_E - 1.0)
	if r.y < 0.0:
		sinh_E = -sinh_E
	var exp_E: float = cosh_E + sinh_E
	var E: float = log( exp_E )
	
	var M: float = abs_e * sinh_E - E
	var periapsis_t = M * n
	
	# Filling in parameters
	args.slr = slr
	args.E   = E
	args.n   = n
	args.periapsis_t = periapsis_t


static func process( dt: float, args: Dictionary ):
	var periapsis_t: float = args.periapsis_t + dt
	args.periapsis_t = periapsis_t
	
	var n: float = args.n
	var M: float = periapsis_t / args.n
	var E: float = solve( args.abs_e, M, args.E )
	
	var coh_E: float = cosh( E )
	var abs_e: float = args.abs_e
	var co_f: float  = ( coh_E - abs_e )/( 1.0 - abs_e*coh_E )
	var si_f: float = sqrt( 1.0 - co_f*co_f )
	if E < 0.0:
		si_f = -si_f
	
	var abs_r = args.slr/(1.0 + abs_e*co_f)
	var x: float = abs_r * co_f
	var y: float = abs_r * si_f
	var r: Vector3 = Vector3( x, y, 0.0 )
	#print( "local r: ", r )
	
	var v: Vector3 = velocity( args, r )
	#print( "local v: ", v )
	
	var A: Basis = args.A
	r = A.xform( r )
	v = A.xform( v )
	
	return [r, v]


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
		En  = ret[1]
		iters += 1
	return En
	


static func solve_next( e: float, M: float, E: float ):
	var sih_E: float = sinh( E )
	var coh_E: float = cosh( E )
	E = E - ( e*sih_E - E - M ) / (e*coh_E - 1.0)
	sih_E = sinh( E )
	var err: float = abs( e*sih_E - E - M )
	return [err, E]




static func velocity( args: Dictionary, r: Vector3 ):
	var gm: float    = args.gm
	var a: float     = args.a
	var abs_e: float = args.abs_e
	var abs_r: float = r.length()
	
	var si_f: float = r.y/abs_r
	var co_f: float = r.x/abs_r
	
	var v: Vector3 = Vector3( -si_f, co_f + abs_e, 0.0 )
	v = v.normalized()
	var abs_v: float = sqrt( gm*( 2.0/abs_r - 1.0/a ) )
	v *= abs_v
	return v




