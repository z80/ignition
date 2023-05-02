extends SubViewportContainer


var _physics_scene: Node = null



func add_physics_body( body: PhysicsBody3D ):
	var p: Node = _get_physics_scene()
	p.add_physics_body( body )


func set_ref_frame( ref_frame: RefFrameNode ):
	var p: Node = _get_physics_scene()
	p.ref_frame = ref_frame


func _get_physics_scene():
	if _physics_scene == null:
		_physics_scene = get_node( "Vp/PhysicsScene" )
	return _physics_scene
