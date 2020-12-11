
const Consts = preload( "res://physics/orbital_motion/constants.gd" )


# Computes (or at least supposed to compute)  velocity vector 
# in orbit plane.
static func velocity( args: Dictionary, f: float ):
	var slr: float   = args.slr
	var abs_e: float = args.abs_e
	
	var si_f: float = sin( f )
	var co_f: float = cos( f )
	
	var denominator: float = 1.0 + abs_e * co_f
	
	# This is just a unit vector along trajectory.
	var k_x: float = slr * si_f * ( abs_e*co_f/denominator - 1.0 ) / denominator
	var k_y: float = slr * ( abs_e * si_f * si_f / denominator + co_f ) / denominator
	var abs_k: float = sqrt( k_x*k_x + k_y*k_y )
	k_x /= abs_k
	k_y /= abs_k
	
	var gm: float = args.gm
	var a: float  = args.a
	var r: float = slr/(1.0 + abs_e*co_f)
	
	var parabolic: bool = (abs_e > (1.0 - Consts.EPS)) and (abs_e < (1.0 + Consts.EPS))
	
	var abs_v: float = speed( gm, a, r, parabolic )
	
	var v_x: float = k_x * abs_v
	var v_y: float = k_y * abs_v
	
	var v: Vector2 = Vector2( v_x, v_y )
	
	return v



# Probably, it should work for all the cases. For elliptic case 
# semimajor axis is positive, so it reduces the velocity.
# And for hyperbolic case semimajor axis is negative, so it adds 
# up to the velocity.
static func speed( gm: float, a: float, r: float, parabolic: bool ):
	var v: float
	if parabolic:
		v = sqrt( 2.0*gm/r )
	else:
		v = sqrt( gm*( 2.0/r - 1.0/a ) )
	return v

