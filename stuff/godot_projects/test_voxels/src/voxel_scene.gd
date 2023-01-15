extends Spatial


export(Resource) var source_resource = null

var _src: VolumeSourceScriptGd = null
var _voxels: MarchingCubesDualGd = null
var _strategy: VolumeNodeSizeStrategyGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_src = source_resource.get_source()
	_voxels = MarchingCubesDualGd.new()
	_strategy = VolumeNodeSizeStrategyGd.new()
	
	_strategy.radius      = 8.0
	_strategy.focal_point = Vector3( 0.0, 0.0, 7.5 )
	_strategy.max_level   = 6
	
	var _min_step: float = _voxels.init_min_step( _src )
	var se3: Se3Ref = Se3Ref.new()
	var ref_pt_se3: Se3Ref = Se3Ref.new()
	var wanted_size: float = 30.0
	
	_voxels.split_precision = -0.001
	
	var bounding_node: BoundingNodeGd = _voxels.create_bounding_node( se3, wanted_size )
	#var ok: bool = _voxels.subdivide_source( bounding_node, _src, _strategy )
	var ok: bool = _voxels.subdivide_source_all( _src, _strategy )
	var qty: int = _voxels.get_nodes_qty()
	
	_voxels.precompute_scaled_values( se3, ref_pt_se3, 0, null )
	var mesh_inst: MeshInstance = get_node( "Mesh" )
	_voxels.apply_to_mesh_only( mesh_inst )
	
#	_voxels.split_precision = 0.001
#
#	bounding_node = bounding_node.create_adjacent_node( -1, 0, 0 )
#	ok = _voxels.subdivide_source( bounding_node, _src )
#	qty = _voxels.get_nodes_qty()
#
#	_voxels.precompute_scaled_values( se3, ref_pt_se3, 0, null )
#	mesh_inst = get_node( "MeshLeft" )
#	_voxels.apply_to_mesh_only_wireframe( mesh_inst )
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_timer():
	var c: Camera = get_node( "Camera" )
	var r: Vector3 = c.global_transform.origin
	r *= 7.0 / r.length()
	
	_strategy.focal_point = r
	
	var se3: Se3Ref = Se3Ref.new()
	var ref_pt_se3: Se3Ref = Se3Ref.new()
	
	_voxels.split_precision = -0.001
	
	#var ok: bool = _voxels.subdivide_source( bounding_node, _src, _strategy )
	var ok: bool = _voxels.subdivide_source_all( _src, _strategy )
	var qty: int = _voxels.get_nodes_qty()
	
	_voxels.precompute_scaled_values( se3, ref_pt_se3, 0, null )
	var mesh_inst: MeshInstance = get_node( "Mesh" )
	_voxels.apply_to_mesh_only_wireframe( mesh_inst )

