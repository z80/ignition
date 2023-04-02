
extends Node3D

@export var update_disabled: bool = false
@export var wireframe: bool = false

# Properties used to initialize strategies.
@export var rebuild_dist: float              = 100.0
@export var rescale_close_dist: float        = 10.0
@export var rescale_far_tangent: float       = 10.0 / 180.0 * 3.14
@export var rescale_depth_rel_tangent: float = 1.0 / 180.0 * 3.14

var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _surfaces: Array = []
var _foliage: Node = null
var _foliage_surface_index: int = -1

# For async run count how many workers running and 
# if zero, re-run with the new se3.
var _async_requested_rebuild: bool = false
var _async_se3: Se3Ref     = null
var _async_workers_qty: int = 0

var async_foliage_ready: bool = true


func _ready():
	_async_se3 = Se3Ref.new()
	_async_workers_qty  = 0
	async_foliage_ready = true
	
	_enumerate_surfaces()
	
	_rebuild_strategy   = MarchingCubesRebuildStrategyGd.new()
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	var surf: Node      = _surfaces[0]

	var radius: float        = surf.get_surface_radius()
	var focus_depth: float   = surf.get_focus_depth()
	var identity_dist: float = surf.get_identity_distance()
	var node_sizes: Array    = surf.get_node_sizes()
	
	_rebuild_strategy.radius = radius
	_rebuild_strategy.height = focus_depth

	_rebuild_strategy.rebuild_dist = rebuild_dist

	_rebuild_strategy.rescale_close_dist        = rescale_close_dist
	_rebuild_strategy.rescale_far_tangent       = rescale_far_tangent
	_rebuild_strategy.rescale_depth_rel_tangent = rescale_depth_rel_tangent
	
	_node_size_strategy.radius = radius
	_node_size_strategy.height = identity_dist
	_node_size_strategy.set_node_sizes( node_sizes[0], node_sizes[1] )


func _process( delta: float ):
	if not update_disabled:
		_async_process()



func _enumerate_surfaces():
	_surfaces = []
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child(i)
		_surfaces.push_back( ch )
		var ch_qty: int = ch.get_child_count()
		if ch_qty > 0:
			_foliage = ch.get_child( 0 )
			_foliage_surface_index = i


func update_source_se3( source_se3: Se3Ref ):
	_async_update_source_se3( source_se3 )
	
	var surface: Node = _surfaces[0]
	var scaler: DistanceScalerBaseRef = RootScene.ref_frame_root.distance_scaler
	
	_update_material_properties( source_se3, scaler )
	
	var t: Transform3D = surface.get_root_se3( source_se3, scaler )
	transform = t


func _update_material_properties( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	var qty: int = _surfaces.size()
	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		surf.update_material_properties( source_se3, scaler )






func _async_update_source_se3( se3: Se3Ref ):
	if update_disabled:
		return
	
	var view_point_se3: Se3Ref = se3.inverse()
	_async_se3.copy_from( se3 )
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if need_rebuild and (not _async_requested_rebuild):
		_async_requested_rebuild = true
		_async_rebuild_start( _async_se3 )




func _async_process():
	if update_disabled or (_async_workers_qty != 0):
		return
	
	elif _async_requested_rebuild:
		_async_rebuild_start( _async_se3 )
	



func _async_rebuild_start( source_se3: Se3Ref ):
	if not _async_requested_rebuild:
		return
	
	if _async_workers_qty != 0:
		return
	
	var scaler: DistanceScalerBaseRef = RootScene.ref_frame_root.distance_scaler
	var qty: int = _surfaces.size()
	
	_async_workers_qty = qty
	
	# Set the focal point.
	var focal_point_rebuild: Vector3 = _rebuild_strategy.get_focal_point_rebuild()
	_node_size_strategy.focal_point = focal_point_rebuild

	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ad: AsyncData = AsyncData.new( source_se3, _node_size_strategy, scaler, surf )
		ad.wireframe     = wireframe
		ad.surface_index = i
		
		#WorkersPool.push_back_with_arg( self, "_async_rebuild_worker", "_async_rebuild_worker_finished", ad )
		var ret = _async_rebuild_worker( ad )
		_async_rebuild_worker_finished( ret )
	



func _async_rebuild_worker( ad: AsyncData ):
	var surf: Node = ad.surface
	var source_se3: Se3Ref = ad.se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = ad.node_size_strategy
	var scaler: DistanceScalerBaseRef = ad.scaler
	ad.callback_data = surf.rebuild_surface( source_se3, node_size_strategy, scaler )
	
	return ad



func _async_rebuild_worker_finished( ad: AsyncData ):
	var surf: Node = ad.surface
	var args: Array = ad.callback_data
	surf.rebuild_surface_finished( args )
	_async_workers_qty -= 1
	
	var worker_index: int = ad.surface_index
	if worker_index == _foliage_surface_index:
		var source_se3: Se3Ref = ad.se3
		var scaler: DistanceScalerBaseRef = ad.scaler
		var se3_at_in_source: Se3Ref = source_se3.inverse()
		var at_in_source: Vector3 = se3_at_in_source.r
		
		false # surf.lock() # TODOConverter40, Image no longer requires locking, `false` helps to not break one line if/else, so it can freely be removed
		
		var foliage_data = _foliage.async_update_population_prepare( source_se3, at_in_source, scaler )
		ad.foliage_data = foliage_data
		
		_foliage.async_populate_node_worker( foliage_data )
		_foliage.async_populate_node_worker_finished( foliage_data )
		
		false # surf.unlock() # TODOConverter40, Image no longer requires locking, `false` helps to not break one line if/else, so it can freely be removed
	
	if _async_workers_qty == 0:
		_async_rescale_start( ad.se3 )




func _async_rescale_start( source_se3: Se3Ref ):
	if _async_workers_qty != 0:
		return

	var scaler: DistanceScalerBaseRef = RootScene.ref_frame_root.distance_scaler
	var qty: int = _surfaces.size()
	
	_async_workers_qty = qty

	# Set the focal point.
	var focal_point_rescale: Vector3 = _rebuild_strategy.get_focal_point_rescale()
	_node_size_strategy.focal_point = focal_point_rescale
	
	
	# Conpute point on the surface (or under the surface) with respect to which 
	# vertices should be computed.
	var surf0: Node   = _surfaces[0]
	var radius: float = surf0.get_surface_radius()
	var local_point_se3: Se3Ref = source_se3.inverse()
	
	# Shouldn't have any rotation.
	local_point_se3.q = Quaternion.IDENTITY
	
	var dist: float = local_point_se3.r.length()
	if dist > radius:
		local_point_se3.r = local_point_se3.r * (radius / dist)


	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ad: AsyncData = AsyncData.new( source_se3, _node_size_strategy, scaler, surf )
		
		ad.wireframe       = wireframe
		ad.surface_index   = i
		ad.local_point_se3 = local_point_se3

		#WorkersPool.push_back_with_arg( self, "_async_rescale_worker", "_async_rescale_worker_finished", ad )
		var ret = _async_rescale_worker( ad )
		_async_rescale_worker_finished( ret )




func _async_rescale_worker( ad: AsyncData ):
	var surf: Node = ad.surface
	var source_se3: Se3Ref = ad.se3
	var local_point_se3: Se3Ref = ad.local_point_se3
	var scaler: DistanceScalerBaseRef = ad.scaler
	surf.rescale_surface( source_se3, local_point_se3, scaler )
	
	return ad



func _async_rescale_worker_finished( ad: AsyncData ):
	var surf: Node = ad.surface
	var wireframe: bool = ad.wireframe
	surf.rescale_surface_finished( wireframe )
	
	_async_workers_qty -= 1
	
	# Disable busyness flag in the very-very end.
	if _async_workers_qty == 0:
		_async_requested_rebuild = false






func get_surface_source():
	var surf: Node = _surfaces[0]
	var ret: Resource = surf.get_surface_source()
	return ret






class AsyncData:
	var wireframe: bool
	var se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var scaler: DistanceScalerBaseRef
	
	var local_point_se3: Se3Ref
	
	var surface: Node
	var callback_data: Array
	
	var surface_index: int
	var foliage_data
	
	func _init( source_se3: Se3Ref, st: VolumeNodeSizeStrategyGd, s: DistanceScalerBaseRef, surf: Node ):
		se3     = Se3Ref.new()
		se3.copy_from( source_se3 )

		node_size_strategy = st

		scaler = s

		surface = surf
		








