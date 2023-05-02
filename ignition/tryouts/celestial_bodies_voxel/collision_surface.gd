
extends StaticBody3D
class_name CollisionSurface

@export var rebuild_dist: float = 100.0: set = _set_rebuild_dist

var _voxel_surface: MarchingCubesDualGd           = null
var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _collision_shape: CollisionShape3D = null

var _is_ready: bool = false
var _initialized_strategy: bool = false
var _is_busy: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	_collision_shape    = get_node( "CollisionShape3D" )
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


func rebuild_surface( ref_frame: RefFrameNode, rotation: RefFrameNode, surface_source: Resource ):
	var p: RefFrameNode = ref_frame.get_parent()
	if (rotation != p):
		if _is_ready:
			# Clean up all the triangles and quit.
			var shape: ConcavePolygonShape3D = _collision_shape.shape
			shape.set_faces( [] )
			# set ready to false
			_is_ready = false
			_initialized_strategy = false

#		return
	
	# Find if subdivision is needed.
	if not _initialized_strategy:
		_initialized_strategy = true
		var r: float = surface_source.source_radius
		_rebuild_strategy.radius = r
		var focus_depth: float = surface_source.focus_depth
		_rebuild_strategy.height = focus_depth
	
	var view_point_se3: Se3Ref = ref_frame.relative_to( rotation )
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if need_rebuild and (not _is_busy):
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
		
		var source_radius: float     = surface_source.source_radius
		var identity_distance: float = surface_source.identity_distance
		var r: Vector3 = view_point_se3.r
		_node_size_strategy.focal_point = r
		_node_size_strategy.radius      = source_radius
		_node_size_strategy.height      = identity_distance
		_node_size_strategy.set_node_sizes( distances, node_sizes )
	
	var data: RebuildData = RebuildData.new()
	data.source_se3     = view_point_se3.inverse()
	data.surface_source = surface_source
	
	# Always update surface synchronously.
	# It is needed if ref frame has teleported.
	# If there is a surface computed and at the same time 
	# there are no asynchronous operations with the 
	# surface builder.
	if _is_ready and (not _is_busy):
		_update_surface_worker( data )
	
	# And now if we have initiate asynchronous surface build.
	if need_rebuild:
		_is_busy = true
		#WorkersPool.push_back_with_arg( self, "_rebuild_surface_worker", "_rebuild_surface_finished", data )
		_rebuild_surface_worker( data )
		_rebuild_surface_finished( data )





func _rebuild_surface_worker( data: RebuildData ):
	print( "_rebuild_surface_worker entered" )
	var surface_source: Resource = data.surface_source
	var source_se3: Se3Ref = data.source_se3
	
	var source_radius: float = surface_source.source_radius
	var source: VolumeSourceGd = surface_source.get_source()
	
	_voxel_surface.max_nodes_qty   = 20000000
	_voxel_surface.split_precision = 0.01
	#var ok: bool = _voxel_surface.subdivide_source( source_radius, source, _node_size_strategy )
	#_voxel_surface.precompute_scaled_values( source_se3, -1, null )
	print( "_rebuild_surface_worker left" )
	return data


func _rebuild_surface_finished( data: RebuildData ):
	_update_surface_worker( data )
	_is_busy = false




class RebuildData:
	var surface_source: Resource
	var source_se3: Se3Ref
	var needs_rebuild: bool





# This one queries triangles closest to the ref. frame center.
func update_surface( ref_frame: RefFrameNode, rotation: RefFrameNode, surface_source: Resource ):
	if (not _is_ready) or _is_busy:
		return
	
	var source_se3: Se3Ref = rotation.relative_to( ref_frame )

	var data: RebuildData = RebuildData.new()
	data.source_se3     = source_se3
	data.surface_source = surface_source
	
	_update_surface_worker( data )




func _update_surface_worker( data: RebuildData ):
	var surface_source: Resource = data.surface_source
	var identity_distance: float = surface_source.identity_distance
	var source_se3: Se3Ref       = data.source_se3
	
	var qty: int = _voxel_surface.get_nodes_qty()
	var collision_triangles: PackedVector3Array = _voxel_surface.collision_faces( source_se3, identity_distance )
	
	print( "Updated surface. Triangles qty: ", collision_triangles.size() )
	
	var shape: ConcavePolygonShape3D = _collision_shape.shape
	shape.set_faces(  collision_triangles )
	_is_ready = true



