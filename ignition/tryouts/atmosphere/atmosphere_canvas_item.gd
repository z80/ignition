extends Control
class_name AtmosphereCanvasItem

export(Vector3) var v_up = Vector3.UP
export(float) var altitude = 0.0
# Towards light.
export(Vector3) var v_light = Vector3( 1.0, 0.0, 0.0 )
export(Color)   var light_color    = Color( 1.0, 1.0, 1.0 )
export(float)   var light_intensity = 10.0
# Ground color, sun color, sun intensity.
export(Color)   var ground_color = Color( 0.0, 1.0, 0.0 )
# Towards line of sight.
export(Vector3) var v_sight = Vector3( 1.0, 0.0, 0.0 )
export(float)   var atm_h    = 5.0
export(float)   var planet_r = 30.0
export(float)   var fov = 135.0/180.0*PI

# Atmosphere parameters.
export(float) var g = 0.4
export(float) var K = 0.1

# Density and height for molecules and particles.
# Should be from 0 to 1.
export(float) var d_m = 1.0
export(float) var h_m = 1.0
export(float) var d_p = 1.0
export(float) var h_p = 1.0

# Attenuation per wavelength.
export(Vector3) var wavelengths = Vector3( 640.0, 550.0, 480.0 )
export(float) var normalizer = 1.0
var exp_coeffs: Vector3

# Approximation. Number of intervals to comute indirect light intensity.
export(int) var intervals = 6


var ground_i: Vector3
var light_i:  Vector3

var vp_size: Vector2
var fx: float = 1.0
var fy: float = 1.0
var cx: float = 0.0
var cy: float = 0.0


# Called when the node enters the scene tree for the first time.
func _ready():
	v_up    = v_up.normalized()
	v_light = v_light.normalized()
	v_sight = v_sight.normalized()
	
	#var vp: Viewport = get_viewport()
	vp_size = get_rect().size #vp.size
	
	# Initialize intrinsic parameters.
	cx = vp_size.x / 2.0
	cy = vp_size.y / 2.0
	
	fx = cx / tan( fov/2.0 )
	var fov_y: float = vp_size.y / vp_size.x * fov
	fy = cy / tan( fov_y/2.0 )
	
	var m = max( wavelengths.x, max( wavelengths.y, wavelengths.z ) )
	var coeff = normalizer * planet_r
	exp_coeffs = Vector3( 1.0/pow(wavelengths.x/m, 4.0) / coeff, 1.0/pow(wavelengths.y/m, 4.0) / coeff, 1.0/pow(wavelengths.z/m, 4.0) / coeff )
	
	ground_i = Vector3( ground_color.r, ground_color.g, ground_color.b )
	light_i  = Vector3( light_color.r * light_intensity, light_color.g * light_intensity, light_color.b * light_intensity )


func _draw():
	draw()



func _camera_basis( up: Vector3, v_sight: Vector3 ):
	# Right, up, back.
	var b: Basis = Basis.IDENTITY
	var q: Quat = _convertion_quat( b.y, up )
	var c: Basis = Basis( q )
	b = c * b
	
	q = _convertion_quat( -b.z, v_sight )
	c = Basis( q )
	b = c * b
	
	return b


func _convertion_quat( from: Vector3, to: Vector3 ):
	var a: Vector3 = from.cross( to )
	var si = a.length()
	if si < 0.001:
		var q: Quat = Quat(a.x/2.0, a.y/2.0, a.z/2.0, 1.0)
		q = q.normalized()
		return q
	
	var ang = asin( si )
	var ang_2 = ang / 2.0
	var si_2 = sin( ang_2 )
	var co_2 = cos( ang_2 )
	a = a.normalized()
	var q: Quat = Quat( a.x*si_2, a.y*si_2, a.z*si_2, co_2 )
	return q


func _pixel_dir( cam_b: Basis, uv: Vector2 ):
	var x: float = (uv.x - cx) / fx
	var y: float = (cy - uv.y) / fy
	var a: Vector3 = Vector3( x, y, -1.0 )
	a = cam_b.xform( a )
	a = a.normalized()
	return a


func draw():
	var r0: Vector3 = v_up * (planet_r + altitude)
	var cam_b : Basis = _camera_basis( v_up, v_sight )
	var sz_u: int = int( vp_size.x )
	var sz_v: int = int( vp_size.y )
	for v in range( sz_v ):
		for u in range( sz_u ):
			var uv: Vector2 = Vector2( u, v )
			var a: Vector3 = _pixel_dir( cam_b, uv )
			var c: Color = color_of_dir( r0, a )
			draw_line( uv, uv+Vector2(1.0, 0.0), c )


func color_of_dir( r0: Vector3, a: Vector3 ):
	var ret: Dictionary = {}
	var ok: bool = _travel_dist( r0, a, ret )
	if not ok:
		return Color( 0.0, 0.0, 0.0, 0.0 )
	
	# Direct light.
	var dist: float = ret.dist
	var opacity: float = 1.0 - exp( -(exp_coeffs.x+exp_coeffs.y+exp_coeffs.z)/3.0*dist )
	
	var color: Vector3 = Vector3.ZERO
	# Scattered light.
	for i in range(intervals):
		var k: float =  float(i)/float(intervals-1)
		# Atmosphere entrance - scattering dist.
		var tb: float = ret.tb
		var at: Vector3 = r0 + a * tb * k
		var scat_ret: Dictionary = {}
		# Distance towards the light.
		ok = _travel_dist( r0, v_light, scat_ret )
		if not ok:
			continue
		# If goes through the surface, ignore. Surface is not 
		# emissive.
		if scat_ret.surf:
			continue
		var straight_dist: float = scat_ret.dist
		var r: float = light_i.x * exp( -exp_coeffs.x*straight_dist )
		var g: float = light_i.y * exp( -exp_coeffs.y*straight_dist )
		var b: float = light_i.z * exp( -exp_coeffs.z*straight_dist )
		# camera - scattering point.
		var scat_cam_dist = dist * k
		r = r * exp( -exp_coeffs.x*scat_cam_dist )
		g = g * exp( -exp_coeffs.y*scat_cam_dist )
		b = b * exp( -exp_coeffs.z*scat_cam_dist )
		color.x += r
		color.y += g
		color.z += b
	
	var m: float = max( color.x, max( color.y, color.z) )
	if m > 0.001:
		color = color / m
	var c: Color = Color( color.x, color.y, color.z, opacity )
	return c


func _travel_dist( r0: Vector3, a: Vector3, ret: Dictionary ):
	# "a" must be unit vector.
	var Ratm: float = planet_r + atm_h
	var ar0: float = a.dot( r0 )
	var aa: float = a.length_squared()
	var r0r0: float = r0.length_squared()
	var RatmRatm: float = Ratm*Ratm
	var D_4atm: float = ar0*ar0 - aa*(r0r0 - RatmRatm)
	# It crosses atmosphere at all if D_4 > 0.
	if D_4atm <= 0.0:
		ret.dist = 0.0
		return false
	
	# At least one should be positive.
	var tb: float = (-ar0 + sqrt(D_4atm)) / aa
	if ( tb <= 0.0 ):
		ret.dist = 0.0
		return false
	
	var ta: float = (-ar0 - sqrt(D_4atm)) / aa
	# If view point is inside, the smallest "t" should be negative, i.e. behind.
	
	# Check if it intersects the surface.
	var Rsurf: float      = planet_r
	var RsurfRsurf: float = Rsurf*Rsurf
	var D_4surf: float = ar0*ar0 - aa*(r0r0 - RsurfRsurf)
	var hits_sphere: bool
	if ( D_4surf >= 0.0 ):
		var t_surf_a: float = (-ar0 - sqrt(D_4surf)) / aa
		#var t_surf_b: float = (-ar0 + sqrt(D_4surf)) / aa
		if (t_surf_a > 0.0) and (tb > t_surf_a):
			tb = t_surf_a
			ret.surf = true
		else:
			ret.surf = false
	else:
		if ta < 0.0:
			ta = 0.0
		ret.surf = false
	
	ret.dist = tb - ta
	ret.ta = ta
	ret.tb = tb
	return true

func phase_r( a: Vector3 ):
	var co: float = a.dot( v_light )
	var ph: float = (1.0-g*g)/pow(1+g*g-2.0*co, 1.5)
	ph /= 4.0*PI
	return ph

func phase_m( a: Vector3 ):
	var co: float = a.dot( v_light )
	var ph: float = 0.75*(1+co*co)
	ph /= 4.0*PI
	return ph
