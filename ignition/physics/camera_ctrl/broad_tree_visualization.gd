
extends MeshInstance3D

@export var enabled: bool = false

var _m: Material = null



# Called when the node enters the scene tree for the first time.
func _ready():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta: float ):
	pass




func draw_faces( broad_tree: BroadTreeGd, camera: RefFrameNode, ref_frame: RefFrameNode ):
	if not enabled:
		return
	
	var im: ImmediateMesh = ImmediateMesh.new()
	
	#var se3: Se3Ref = camera.relative_to( ref_frame )
	var se3: Se3Ref = ref_frame.relative_to( camera )
	var t: Transform3D = se3.transform
	var faces: PackedVector3Array = broad_tree.face_lines( t )
	var qty: int = faces.size()
	
	im.clear_surfaces()
	im.surface_begin( ImmediateMesh.PRIMITIVE_LINES, _m )
	
	for i in range( qty ):
		var at: Vector3 = faces[i]
		im.surface_add_vertex( at )
	
	im.surface_end()
	
	self.mesh = im











