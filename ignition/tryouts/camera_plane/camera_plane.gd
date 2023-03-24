tool
extends Camera

export(float) var dr = 1.0

var _q: Quat
var plane: Spatial = null


func set_viewport( vp: Viewport ):
	var mi: MeshInstance = plane
	var m: SpatialMaterial = mi.get_surface_material( 0 )
	var tex: ViewportTexture = m.albedo_texture
	tex.viewport_path = vp.get_path()
	m.albedo_texture = tex


# Called when the node enters the scene tree for the first time.
func _ready():
	var q2: Quat = Quat( Vector3.UP, deg2rad(180.0) )
	var q1: Quat = Quat( Vector3.RIGHT, deg2rad(-90.0) )
	_q = q1 * q2
	
	plane = get_child( 0 )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var cam: Camera = self
	var dist: float = cam.far - dr
	var vp: Viewport = get_viewport()
	var sz: Vector2 = vp.size
	
	var a: Vector3 = cam.project_local_ray_normal( Vector2.ZERO )
	var b: Vector3 = cam.project_local_ray_normal( sz )
	a = (b - a) * dist
	
	var sc: Vector3 = Vector3( a.x, 1.0, a.y )
	var bs: Basis = Basis.IDENTITY
	bs = bs.scaled( sc )
	var b0: Basis = Basis( _q )
	bs = b0 * bs
	
	var t: Transform = Transform( bs, Vector3( 0.0, 0.0, -dist ) )
	plane.transform = t
	
	#print( t )
