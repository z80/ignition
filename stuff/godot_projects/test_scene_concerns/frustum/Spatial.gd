extends Spatial


export(PackedScene) var Box = null


func create_boxes():
	var cam: Camera = get_node( "Camera" )
	var far: float = cam.far
	var step: float = 3.0
	var qty: int = int( far / step )
	for i in range(qty):
		var z: float = far * float( i ) / float(qty-1)
		var m: Spatial = Box.instance()
		add_child( m )
		var t: Transform = m.transform
		t.origin = Vector3( 0.0, -2.5, -z )
		m.transform = t
	
	adjust_scale_to_camera()
	create_tween()

func create_tween():
	var cam: Camera = get_node( "Camera" )
	var far: float = cam.far * 1.2

	var p: Spatial = get_node( "Plane" )
	var t: SceneTreeTween = get_tree().create_tween()
	t.tween_property( p, "translation", Vector3(0.0, 0.0, 0.0), 0.1 ).set_trans(Tween.TRANS_LINEAR )
	for i in range( 10 ):
		t.tween_property( p, "translation", Vector3(0.0, 0.0, -far), 5.0 ).set_trans(Tween.TRANS_LINEAR )
		t.tween_property( p, "translation", Vector3(0.0, 0.0, 0.0), 5.0 ).set_trans(Tween.TRANS_LINEAR )


func adjust_scale_to_camera():
	var cam: Camera = get_node( "Camera" )
	var far: float = cam.far
	var vp: Viewport = cam.get_viewport()
	var sz: Vector2 = vp.size

	var a: Vector3 = cam.project_local_ray_normal( Vector2.ZERO )
	var b: Vector3 = cam.project_local_ray_normal( sz )
	a = (a - b) * far * 2.0
	a.x = abs(a.x)
	a.y = abs(a.y)
	
	var p: Spatial = get_node( "Plane" )
	p.transform = p.transform.scaled( Vector3(a.x, a.y, 1.0 ) )
	
	


# Called when the node enters the scene tree for the first time.
func _ready():
	call_deferred( "create_boxes" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
