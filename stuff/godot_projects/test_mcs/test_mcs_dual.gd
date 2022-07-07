extends Spatial

var source_tree: VolumeSourceTreeGd  = null
var source: VolumeSourceScriptGd     = null
var cubes: MarchingCubesDualGd = null
var scaler: DistanceScalerRef = null

var meshes: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	meshes = [ get_node("Mesh_0" ), get_node("Mesh_1") ]
	
#	scaler = DistanceScalerRef.new()
#	scaler.plain_distance = 150.0
#	scaler.log_scale      = 10.0
	
	
	
	#source_tree = VolumeSourceTreeGd.new()
	
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://volume_source_sphere_3.gd" )
	source.set_script( script )
	source.bounding_radius = 11.0
	source.radius          = 10.0
	source.node_sz         = 1.0
	source.material_index  = 0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	source.se3 = se3
	
	


	cubes = MarchingCubesDualGd.new()
	cubes.max_nodes_qty = 200000
	se3.r = Vector3( 0.0, 0.0, 0.0 )
	cubes.set_source_transform( se3 )

	cubes.subdivide_source( 11.0, source, scaler )
	
	_draw_octree_nodes()
	_draw_dual_cells()
	
	var material_inds: Array = cubes.materials_used()
	
	for mi in meshes:
		mi.visible = false
	
	for ind in material_inds:
		if ind < 0:
			ind = 0
		var m: MeshInstance = meshes[ind]
		m.visible = true
		#cubes.apply_to_mesh( ind, m, null )
		cubes.apply_to_mesh( ind, m, scaler )





func _draw_octree_nodes():
	var drawer: NodeDrawer = get_node("OctreeNodes")
	
	drawer.clear_nodes()
	
	var qty: int = cubes.get_nodes_qty()
	for i in range(qty):
		var n: Array = cubes.get_node( i )
		drawer.add_node( n )
	
	drawer.draw()



func _draw_dual_cells():
	var drawer: NodeDrawer = get_node("DualCells")
	
	drawer.clear_nodes()
	
	var qty: int = cubes.get_dual_cells_qty()
	for i in range(qty):
		var n: Array = cubes.get_dual_cell( i )
		drawer.add_node( n )
	
	drawer.draw()






