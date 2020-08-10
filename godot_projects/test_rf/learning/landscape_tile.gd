extends MeshInstance

var index_x: int = 0
var index_z: int = 0
var rebuild: bool = true

# Called when the node enters the scene tree for the first time.
func _ready():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func _construct():
	var ls = get_parent()
	if not ls:
		return
	#if not (ls is MeshInstance):
	#	return
	var origin_x: float = ls.size * float(index_x)
	var origin_z: float = ls.size * float(index_z)
	var verts = []
	for iz in range( ls.resolution ):
		var z0 = 0.7 * float(iz) * ls.size / float(ls.resolution) + origin_z
		var z1 = 0.7 * float(iz+1) * ls.size / float(ls.resolution) + origin_z
		for ix in range( ls.resolution ):
			var x0 = 0.7 * float(ix) * ls.size / float(ls.resolution) + origin_x
			var x1 = 0.7 * float(ix+1) * ls.size / float(ls.resolution) + origin_x
			var h0: float = ls.height( x0, z0 )
			var h1: float = ls.height( x1, z0 )
			var h2: float = ls.height( x1, z1 )
			var h3: float = ls.height( x0, z1 )
			
			var v0 := Vector3( x0, h0, z0 )
			var v1 := Vector3( x1, h1, z0 )
			var v2 := Vector3( x1, h2, z1 )
			var v3 := Vector3( x0, h3, z1 )
			
			verts.push_back( v0 )
			verts.push_back( v1 )
			verts.push_back( v3 )
			
			verts.push_back( v1 )
			verts.push_back( v2 )
			verts.push_back( v3 )
	
	var vertices = PoolVector3Array()
	var sz = verts.size()
	vertices.resize( sz )
	for i in range( sz ):
		vertices[i] = verts[i]
	
	
	# Initialize the ArrayMesh.
	var arr_mesh = ArrayMesh.new()
	var arrays = []
	arrays.resize(ArrayMesh.ARRAY_MAX)
	arrays[ArrayMesh.ARRAY_VERTEX] = vertices
	# Create the Mesh.
	arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	#arr_mesh.surface_set_material( 0 )
	#var m = MeshInstance.new()
	self.mesh = arr_mesh
	

	# Create water
	for i in range( sz ):
		var v: Vector3 = verts[i]
		v.y = 0.0
		vertices[i] = v
	arr_mesh = ArrayMesh.new()
	arrays = []
	arrays.resize(ArrayMesh.ARRAY_MAX)
	arrays[ArrayMesh.ARRAY_VERTEX] = vertices
	# Create the Mesh.
	arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	#arr_mesh.surface_set_material( 0 )
	#var m = MeshInstance.new()
	$WaterMesh.mesh = arr_mesh
	
	
	# Create collision.
	var editor_sh = $RigidBody/CollisionShape
	var sh: Shape = editor_sh.shape
	sh.set_faces( vertices )
	
	rebuild = false






