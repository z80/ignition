
extends Node
class_name CollisionSurface

export(float) var rebuild_dist = 100.0 setget _set_rebuild_dist

var _voxel_surface: MarchingCubesDualGd           = null
var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _collision_shape: CollisionShape = null

var _is_ready: bool = false
var _initialized_strategy: bool = false
var _is_busy: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	_collision_shape    = get_node( "CollisionShape" )
	_is_ready = false
	_initialized_strategy = false
	
	_voxel_surface      = MarchingCubesDualGd.new()
	_rebuild_strategy   = MarchingCubesRebuildStrategyGd.new()
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()


# This one is to make it updated live if the parameter is changed 
# while the game is running.
func _set_rebuild_dist( d: float ):
	rebuild_dist = d
	if _rebuild_strategy != null:
		_rebuild_strategy.rebuild_dist = rebuild_dist


func rebuild_surface( ref_frame: RefFrameNode, planet: RefFrameNode, surface_source: Resource ):
	var rotation: RefFrameNode = planet.rotation_rf()
	var p: RefFrameNode = ref_frame.get_parent()
	if (rotation != p):
		if _is_ready:
			# Clean up all the triangles and quit.
			var shape: ConcavePolygonShape = _collision_shape.shape
			shape.set_faces( [] )
			# set ready to false
			_is_ready = false
			_initialized_strategy = false

		return
	
	# Find if subdivision is needed.
	if not _initialized_strategy:
		var r: float = surface_source.source_radius
		_rebuild_strategy.radius = r
		var focus_depth: float = surface_source.focus_depth
		_rebuild_strategy.height = focus_depth
	
	var view_point_se3: Se3Ref = ref_frame.relative_to( planet )
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if need_rebuild and (not _is_busy):
		_is_busy = true
		pass


func _rebuild_surface_start( surface_source: Resource, source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
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
	var ok: bool = _voxel_surface.subdivide_source( source_radius, source, _node_size_strategy )
	var ret: Array = [ok, source_se3, scaler]

	return ret


func _rebuild_surface_worker_process( source_radius: float, surface_source: Resource ):
	var source: VolumeSourceGd   = surface_source.get_source()
	var ok: bool = _voxel_surface.subdivide_source( source_radius, source, _node_size_strategy )


func rebuild_surface_finished( data: Array ):
	_is_busy = false
	_is_ready = true
	var collision_triangles: PoolVector3Array = _voxel_surface.collision_faces()
	var shape: ConcavePolygonShape = _collision_shape.shape
	shape.set_faces(  collision_triangles )
	_is_busy = false
	_is_ready = true




class RebuildData:
	var surface_source: Resource
	var source_se3: Se3Ref








