extends Camera3D

@export var dr: float = 1.0

var _q: Quaternion
var plane: Node3D = null


func set_viewport( vp: SubViewport ):
	var mi: MeshInstance3D = plane
	var m: StandardMaterial3D = mi.get_surface_override_material( 0 )
	var tex: ViewportTexture = m.albedo_texture
	tex.viewport_path = vp.get_path()
	m.albedo_texture = tex


# Called when the node enters the scene tree for the first time.
func _ready():
	var q2: Quaternion = Quaternion( Vector3.UP, deg_to_rad(0.0) )
	var q1: Quaternion = Quaternion( Vector3.RIGHT, deg_to_rad(45.0) )
	_q = q1 * q2
	
	plane = get_child( 0 )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var q2: Quaternion = Quaternion( Vector3.UP, deg_to_rad(0.0) )
	var q1: Quaternion = Quaternion( Vector3.RIGHT, deg_to_rad(-90.0) )
	_q = q1 * q2


	var cam: Camera3D = self
	var dist: float = cam.far
	var vp: SubViewport = get_viewport()
	var sz: Vector2 = vp.size
	
	var a: Vector3 = cam.project_local_ray_normal( Vector2.ZERO )
	var b: Vector3 = cam.project_local_ray_normal( sz )
	var coeff: float = 0.5
	a = (b - a) * dist * 2.0 * coeff
	a.x = abs(a.x)
	a.y = abs(a.y)
	
	var sc: Vector3 = Vector3( a.x, 1.0, a.y )
	var bs: Basis = Basis.IDENTITY
	bs = bs.scaled( sc )
	var b0: Basis = Basis( _q )
	bs = b0 * bs
	
	var t: Transform3D = Transform3D( bs, Vector3( 0.0, 0.0, -dist*coeff ) )
	#var t: Transform = Transform( bs, Vector3( 0.0, 0.0, -10.0 ) )
	plane.transform = t
	
	#print( t )
