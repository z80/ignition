extends Spatial


export(Resource) var source_resource = null

var _src: VolumeSourceScriptGd = null
var _voxels: MarchingCubesDualGd = null
var _strategy: VolumeNodeSizeStrategyGd = null
var _rel_se3: Se3Ref = null
var _scale_dist_ratio: ScaleDistanceRatioGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_src      = source_resource.get_source()
	_voxels   = MarchingCubesDualGd.new()
	_strategy = VolumeNodeSizeStrategyGd.new()
	_rel_se3  = Se3Ref.new()
	_scale_dist_ratio = ScaleDistanceRatioGd.new()
	_scale_dist_ratio.max_distance = 200.0
	
	_strategy.radius      = 8.0
	_strategy.focal_point = Vector3( 0.0, 0.0, 7.99 )
	_strategy.max_level   = 3
	
	var _min_step: float = _voxels.init_min_step( _src )
	var se3: Se3Ref = Se3Ref.new()
	var ref_pt_se3: Se3Ref = Se3Ref.new()
	var wanted_size: float = 30.0
	
	_voxels.split_precision = -0.001
	
	var bounding_node: BoundingNodeGd = _voxels.create_bounding_node( se3, wanted_size )
	#var ok: bool = _voxels.subdivide_source( bounding_node, _src, _strategy )
	var ok: bool = _voxels.subdivide_source_all( _src, _strategy )
	var qty: int = _voxels.get_nodes_qty()
	
	_voxels.precompute_scaled_values( se3, ref_pt_se3, 0, 1.0 )
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

	regenerate_mesh()


func _process( _delta ):
	apply_transform()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func apply_transform():
	var mesh_inst: MeshInstance = get_node( "Mesh" )
	var cam: Camera = get_node("Camera")
	var t: Transform = cam.global_transform.inverse()
	_rel_se3.transform = t
	var tt: Transform = _scale_dist_ratio.compute_transform( _rel_se3, 2.0 )
	tt = cam.global_transform * tt
	mesh_inst.global_transform = tt


func _on_timer():
	pass
	

func regenerate_mesh():
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
	
	_voxels.precompute_scaled_values( se3, ref_pt_se3, 0, 2.0 )
	var mesh_inst: MeshInstance = get_node( "Mesh" )
	_voxels.apply_to_mesh_only_wireframe( mesh_inst )

	mesh_inst = get_node( "MeshLeft" )
	_voxels.apply_to_mesh_only_wireframe( mesh_inst )
	
	apply_transform()

