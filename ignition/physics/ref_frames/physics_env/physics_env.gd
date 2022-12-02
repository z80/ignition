extends ViewportContainer


var _physics_scene: Node = null


# Called when the node enters the scene tree for the first time.
func _init():
	_physics_scene = get_node( "Vp/PhysicsScene" )


func add_physics_body( body: PhysicsBody ):
	_physics_scene.add_physics_body( body )


func set_ref_frame( ref_frame: RefFrameNode ):
	if _physics_scene == null:
		_physics_scene = get_node( "Vp/PhysicsScene" )
	_physics_scene.ref_frame = ref_frame
