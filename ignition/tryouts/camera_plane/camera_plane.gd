extends Camera3D

@export var dr: float = 1.0

var _q: Quaternion
var plane: Node3D = null


func setup_viewport( vp: SubViewport ):
	var mi: MeshInstance3D = plane
	var m: StandardMaterial3D = mi.material_override
	var tex: ViewportTexture = m.albedo_texture
	tex.viewport_path = vp.get_path()
	m.albedo_texture = tex


# Called when the node enters the scene tree for the first time.
func _ready():
	plane = get_child( 0 )
	_q = Quaternion( Vector3.RIGHT, deg_to_rad(90.0) )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):


	var cam: Camera3D = self
	var dist: float = cam.far
	var vp: Window = get_viewport()
	var sz: Vector2 = vp.size
	
	var a: Vector3 = cam.project_local_ray_normal( Vector2.ZERO )
	var b: Vector3 = cam.project_local_ray_normal( sz )
	
	var coeff: float = 0.5
	a = (b - a) * dist * 2.0 * coeff * 1.0
	a.x = abs(a.x)
	a.y = abs(a.y)
	
	var sc: Vector3 = Vector3( a.x, 1.0, a.y )
	var bs: Basis = Basis.IDENTITY
	bs = bs.scaled( sc )
	var b0: Basis = Basis( _q )
	bs = b0 * bs
	
	var t: Transform3D = Transform3D( bs, Vector3( 0.0, 0.0, -dist*coeff ) )
	plane.transform = t
	#t = Transform3D( b0, Vector3( 0.0, 0.0, -10.0 ) )
	#plane.transform = t
	
	#print( t )
