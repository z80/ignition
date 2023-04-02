extends MeshInstance3D

var Landscape = preload( "res://learning/landscape.tscn" )

var landscape = null
var index_x: int = 0
var index_z: int = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	_construct()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func _construct():
	var ls = get_parent()
	if not ls:
		return
	if not (ls is Landscape):
		return
	var sz_2: float = ls.size * 0.5 * 0.0
	var verts = []
	for iy in range( ls.resolution ):
		var y0 = float(iy) * ls.size / float(ls.resolution) - sz_2
		var y1 = float(iy+1) * ls.size / float(ls.resolution) - sz_2
		for ix in range( ls.resolution ):
			var x0 = float(ix) * ls.size / float(ls.resolution) - sz_2
			var x1 = float(ix+1) * ls.size / float(ls.resolution) - sz_2
			var v0 := Vector3( x0, 0.0, y0 )
			var v1 := Vector3( x1, 0.0, y0 )
			var v2 := Vector3( x1, 0.0, y1 )
			var v3 := Vector3( x0, 0.0, y1 )
			
			verts.push_back( v0 )
			verts.push_back( v1 )
			verts.push_back( v3 )
			
			verts.push_back( v1 )
			verts.push_back( v2 )
			verts.push_back( v3 )
	
	var vertices = PackedVector3Array()
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



func _inside( at: Vector3 ):
	var x = float(index_x) * size
	var z = float(index_z) * size
	if x > at.x:
		return false
	if z > at.z:
		return false
	if at.x > x+size:
		return false
	if at.z > z+size:
		return false
		
	return true



func _dist( at: Vector3 ):
	var ix := int( floor( at.x/size ) )
	var iz := int( floor( at.z/size ) )
	var dx := index_x - ix
	var dz := index_z - iz
	if dx < 0:
		dx = -dx
	if dz < 0:
		dz = -dz
	
	if dx <= dz:
		return dx
	
	return dz
	
	

