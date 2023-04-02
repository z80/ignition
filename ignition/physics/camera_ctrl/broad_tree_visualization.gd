extends ImmediateMesh

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
	
	#var se3: Se3Ref = camera.relative_to( ref_frame )
	var se3: Se3Ref = ref_frame.relative_to( camera )
	var t: Transform3D = se3.transform
	var faces: PackedVector3Array = broad_tree.face_lines( t )
	var qty: int = faces.size()
	
	set_material_override( _m )
	clear()
	begin( Mesh.PRIMITIVE_LINES )
	
	for i in range( qty ):
		var at: Vector3 = faces[i]
		add_vertex( at )
	
	end()










