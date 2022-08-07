extends Spatial

var source_tree: VolumeSourceTreeGd  = null
var source: VolumeSourceScriptGd     = null
var cubes: MarchingCubesDualGd = null
var scaler: DistanceScalerRef = null

var spatial: Spatial = null
var meshes: Array = []

# Called when the node enters the scene tree for the first time.
func _ready():
	spatial = get_node( "Spatial" )
	meshes  = [ get_node("Spatial/Mesh_0" ), get_node("Spatial/Mesh_1") ]
	
	scaler = DistanceScalerRef.new()
	scaler.plain_distance = 150.0
	scaler.log_scale      = 10.0
	
	
	
	source_tree = VolumeSourceTreeGd.new()
	
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://filling_nodes/volume_source_sphere.gd" )
	source.set_script( script )
	source.bounding_radius = 450.0
	source.radius = 400.0
	source.node_sz_max = 200.0
	source.node_sz_min = 1.0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	source.se3 = se3
	
	source.material_index = 0
	


	
	


	cubes = MarchingCubesDualGd.new()
	cubes.max_nodes_qty = 20000000
	se3.r = Vector3( 0.0, 0.0, -400.0 )
	cubes.set_source_transform( se3 )

	cubes.split_precision = 0.01
	var _ok: bool = cubes.subdivide_source( 1200.0, source, scaler )
	
	var nodes_qty: int = cubes.get_nodes_qty()
	var cells_qty: int = cubes.get_dual_cells_qty()
	
	print( "nodes: ", nodes_qty, "; cells: ", cells_qty )
	
#	_draw_octree_nodes()
	#_draw_dual_cells()
	
	var material_inds: Array = cubes.materials_used()
	
	for mi in meshes:
		mi.visible = false
	
	for ind in material_inds:
		if ind < 0:
			ind = 0
		var m: MeshInstance = meshes[ind]
		m.visible = true
		cubes.apply_to_mesh( ind, m, scaler )
	
	spatial.transform = cubes.mesh_transform( scaler )
	
	
	#_query_close_nodes()
	
	var populator: Node = get_node( "Populator" )
	se3 = Se3Ref.new()
	populator.update( spatial, cubes, se3, scaler )




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




func _query_close_nodes():
	var inds: Array = cubes.query_close_nodes( Vector3.ZERO, 50.0, 64.0 )
	print( "close indices: ", inds )
	
	var centers: Array = []
	var hashes: Array  = []
	for ind in inds:
		var n: MarchingCubesDualNodeGd = cubes.get_tree_node( ind )
		var c: Vector3 = n.center_vector( false )
		centers.push_back( c )
		
		var h: String = n.hash()
		hashes.push_back( h )
	
	print( "centers: ", centers )
	print( "hashes:  ", hashes )






