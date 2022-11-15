
extends Spatial

export(Resource) var surface_source = null

var _voxel_surface: MarchingCubesDualGd           = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_voxel_surface      = MarchingCubesDualGd.new()
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()




func rebuild_surface( surface_source: Resource, source_se3: Se3Ref, strategy: VolumeNodeSizeStrategyGd, scaler: DistanceScalerBaseRef ):
	var source_radius: float     = surface_source.source_radius
	var identity_distance: float = surface_source.identity_distance
	var source: VolumeSourceGd   = surface_source.get_source()
	
	var node_dicts: Array = surface_source.node_sizes
	var qty: int = node_dicts.size()
	var distances: Array  = []
	var node_sizes: Array = []
	for i in range(qty):
		var vv: Dictionary = node_dicts[i]
		var dist: float = vv.distance
		var sz: float   = vv.node_size
		distances.push_back( dist )
		node_sizes.push_back( sz )
	
	var inv_source_se3: Se3Ref = source_se3.inverse()
	var r: Vector3 = inv_source_se3.r
	_node_size_strategy.focal_point = r
	_node_size_strategy.radius      = source_radius
	_node_size_strategy.height      = identity_distance
	_node_size_strategy.set_node_sizes( distances, node_sizes )

	
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.split_precision = 0.01
	var ok: bool = _voxel_surface.subdivide_source( source_radius, source, strategy )
	var ret: Array = [ok, source_se3, scaler]

	return ret


func rebuild_surface_finished( data: Array ):
	pass









