extends Spatial

var _camera: Camera = null
var ref_frame: RefFrameNode = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_camera = get_node( "Camera" )



func _process(_delta):
	_align_camera()


func add_physics_body( body: PhysicsBody ):
	var p: Node = body.get_parent()
	if p != null:
		p.remove_child( body )
	
	add_child( body )
	body.owner = self


func _align_camera():
	if ref_frame == null:
		return
	
	var c: RefFrameNode = PhysicsManager.camera
	var se3: Se3Ref     = c.relative_to( ref_frame )
	var t: Transform    = se3.transform
	_camera.transform   = t



