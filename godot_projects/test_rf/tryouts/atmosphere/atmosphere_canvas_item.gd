extends CanvasItem
class_name AtmosphereCanvasItem

export(Vector3) var v_up = Vector3.UP
# Towards light.
export(Vector3) var v_light = Vector3( 1.0, 1.0, 1.0 )
# Towards line of sight.
export(Vector3) var v_sight = Vector3( 1.0, 1.0, 1.0 )
export(float) var atm_h    = 5.0
export(float) var planer_r = 30.0
export(float) var fov = 135.0/180.0*PI

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
	
	var vp: Viewport = get_viewport()
	vp_size = vp.size
	
	# Initialize intrinsic parameters.
	cx = vp_size.x / 2.0
	cy = vp_size.y / 2.0
	
	fx = cx / tan( fov/2.0 )
	var fov_y: float = vp_size.y / vp_size.x * fov
	fy = cy / tan( fov_y/2.0 )
	


func _draw():
	pass



func _camera_basis( up: Vector3, v_sight: Vector3 ):
	# Right, up, back.
	var b: Basis = Basis.IDENTITY
	var q: Quat = convertion_quat( b.y, up )
	var c: Basis = Basis( q )
	b = c * b
	
	q = convertion_quat( -b.z, v_sight )
	c = Basis( q )
	b = c * b
	
	return b


func convertion_quat( from: Vector3, to: Vector3 ):
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


