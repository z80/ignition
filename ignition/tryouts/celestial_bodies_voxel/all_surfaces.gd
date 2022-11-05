
extends Spatial

export(bool) var synchronous_update = true

# Properties used to initialize strategies.
export(float) var focus_depth               = 40.0
export(float) var rebuild_dist              = 100.0
export(float) var rescale_close_dist        = 10.0
export(float) var rescale_far_tangent       = 10.0 / 180.0 * 3.14
export(float) var rescale_depth_rel_tangent = 1.0 / 180.0 * 3.14


var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd

var _surfaces: Array = []
var _foliage: Spatial = null
var _foliage_surface_index: int = -1

# For async run count how many workers running and 
# if zero, re-run with the new se3.
var _async_requested_rebuild: bool = false
var _async_requested_rescale: bool = false
var _async_se3: Se3Ref     = null
var _async_workers_qty: int = 0

var async_foliage_ready: bool = true


func _ready():
	_async_se3 = Se3Ref.new()
	_async_workers_qty  = 0
	async_foliage_ready = true

	_enumerate_surfaces()
	
	_rebuild_strategy = MarchingCubesRebuildStrategyGd.new()
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	var surf: Node = _surfaces[0]
	var radius: float = surf.get_surface_radius()
	
	_rebuild_strategy.radius = radius
	_rebuild_strategy.height = focus_depth

	_rebuild_strategy.rebuild_dist = rebuild_dist

	_rebuild_strategy.rescale_close_dist        = rescale_close_dist
	_rebuild_strategy.rescale_far_tangent       = rescale_far_tangent
	_rebuild_strategy.rescale_depth_rel_tangent = rescale_depth_rel_tangent
	
	_node_size_strategy.radius = radius
	_node_size_strategy.height = focus_depth


func _process( delta: float ):
	if not synchronous_update:
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
	if synchronous_update:
		_sync_update_source_se3( source_se3 )
	
	else:
		_async_update_source_se3( source_se3 )
	
	var surface: Node = _surfaces[0]
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var t: Transform = surface.get_root_se3( source_se3, scaler )
	transform = t



func _sync_update_source_se3( source_se3: Se3Ref ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var surface: Node = _surfaces[0]

	var view_point_se3: Se3Ref = source_se3.inverse()
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	var r: Vector3 = view_point_se3.r
	DDD.important()
	DDD.print( "r: " + str( r ), 5.0, "aaa" )
	if need_rebuild:
		DDD.print( "need rebuild: " + str( view_point_se3.r ) )
		_node_size_strategy.focal_point = _rebuild_strategy.get_focal_point_rebuild()
	
	var need_rescale: bool
	if need_rebuild:
		need_rescale = false
	else:
		need_rescale = _rebuild_strategy.need_rescale( view_point_se3 )
		if need_rescale:
			_node_size_strategy.focal_point = _rebuild_strategy.get_focal_point_rescale()
	
	if need_rebuild:
		# If needed rebuild, rebuild voxel surface and apply to meshes.
		var data: Array = _rebuild( source_se3 )
		_rebuild_finished( data )
	
	# Else only apply to meshes without applying to the surface.
	if need_rebuild or need_rescale:
		_rescale( source_se3 )
		_rescale_finished()
	



# Rebuild voxel representation and reapply to meshes.
func _rebuild( source_se3: Se3Ref ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ret: Array = surf.rebuild_surface( source_se3, _node_size_strategy, scaler )
		data.push_back( ret )
	
#	var point_se3: Se3Ref = source_se3.inverse()
#	_foliage.update_population( point_se3, scaler, true )
	return data


func _rebuild_finished( data: Array ):
	var qty: int = _surfaces.size()
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var args: Array = data[i]
		surf.rebuild_surface_finished( args )



# Just re-apply to meshes without rebuilding voxel surface.
func _rescale( source_se3: Se3Ref ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	for i in range(qty):
		var surf: Node = _surfaces[i]
		surf.rescale_surface( source_se3, scaler )
#	_foliage.update_view_point( source_se3, scaler )


func _rescale_finished():
	var qty: int = _surfaces.size()
	for i in range(qty):
		var surf: Node = _surfaces[i]
		surf.rescale_surface_finished()




func _async_update_source_se3( se3: Se3Ref ):
	var view_point_se3: Se3Ref = se3.inverse()
	_async_se3.copy_from( se3 )
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if need_rebuild and (not _async_requested_rebuild):
		_async_requested_rebuild = true
		_async_rebuild_start( _async_se3 )
	
	if not _async_requested_rebuild:
		var need_rescale: bool = _rebuild_strategy.need_rescale( view_point_se3 )
		if need_rescale and (not _async_requested_rescale):
			_async_requested_rescale = need_rescale
			_async_rescale_start( _async_se3 )



func _async_process():
	if synchronous_update or (_async_workers_qty != 0):
		return
	
	if _async_requested_rebuild:
		_async_rebuild_start( _async_se3 )
	
	elif _async_requested_rescale:
		_async_rescale_start( _async_se3 )



func _async_rebuild_start( source_se3: Se3Ref ):
	if not _async_requested_rebuild:
		return
	
	if _async_workers_qty != 0:
		return
	
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	
	_async_requested_rebuild = false
	_async_workers_qty = qty

	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ad: AsyncData = AsyncData.new( source_se3, _node_size_strategy, scaler, surf )
		ad.surface_index = i
		
#		if i == _foliage_surface_index:
#			var se3_at_in_source: Se3Ref = source_se3.inverse()
#			var at_in_source: Vector3 = se3_at_in_source.r
#			var foliage_data = _foliage.async_update_population_prepare( self, at_in_source, scaler )
#			ad.foliage_data = foliage_data
		
		WorkersPool.push_back_with_arg( self, "_async_rebuild_worker", "_async_rebuild_worker_finished", ad )
	



func _async_rebuild_worker( ad: AsyncData ):
	var surf: Node = ad.surface
	var source_se3: Se3Ref = ad.se3
	var node_size_strategy: VolumeNodeSizeStrategyGd = ad.node_size_strategy
	var scaler: DistanceScalerBaseRef = ad.scaler
	ad.callback_data = surf.rebuild_surface( source_se3, node_size_strategy, scaler )
	
#	var worker_index: int = ad.surface_index
#	if worker_index == _foliage_surface_index:
#		var foliage_data = ad.foliage_data
#		_foliage.async_populate_node_worker( foliage_data )
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
		var foliage_data = _foliage.async_update_population_prepare( self, at_in_source, scaler )
		ad.foliage_data = foliage_data
		
		_foliage.async_populate_node_worker( foliage_data )
		
		_foliage.async_populate_node_worker_finished( foliage_data )
	
	if _async_workers_qty == 0:
		_async_requested_rescale = true




func _async_rescale_start( source_se3: Se3Ref ):
	if not _async_requested_rescale:
		return
	
	if _async_workers_qty != 0:
		return

	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	
	_async_requested_rescale = false
	_async_workers_qty = qty


	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ad: AsyncData = AsyncData.new( source_se3, _node_size_strategy, scaler, surf )
		ad.surface_index = i
#		if i == _foliage_surface_index:
#			var foliage_data = _foliage.async_update_view_point_prepare( source_se3, scaler )
#			ad.foliage_data = foliage_data

		WorkersPool.push_back_with_arg( self, "_async_rescale_worker", "_async_rescale_worker_finished", ad )




func _async_rescale_worker( ad: AsyncData ):
	var surf: Node = ad.surface
	var source_se3: Se3Ref = ad.se3
	var scaler: DistanceScalerBaseRef = ad.scaler
	surf.rescale_surface( source_se3, scaler )
	
#	var surface_index: int = ad.surface_index
#	if surface_index == _foliage_surface_index:
#		var foliage_data = ad.foliage_data
#		_foliage.async_update_view_point_worker( foliage_data )
	
	return ad



func _async_rescale_worker_finished( ad: AsyncData ):
	var surf: Node = ad.surface
	surf.rescale_surface_finished()

	var surface_index: int = ad.surface_index
	if surface_index == _foliage_surface_index:
		var source_se3: Se3Ref            = ad.se3
		var scaler: DistanceScalerBaseRef = ad.scaler
		var foliage_data = _foliage.async_update_view_point_prepare( source_se3, scaler )
		_foliage.async_update_view_point_worker( foliage_data )
#		var foliage_data = ad.foliage_data
		_foliage.async_update_view_point_worker( foliage_data )

	_async_workers_qty -= 1







class AsyncData:
	var se3: Se3Ref
	var node_size_strategy: VolumeNodeSizeStrategyGd
	var scaler: DistanceScalerBaseRef
	
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
		



