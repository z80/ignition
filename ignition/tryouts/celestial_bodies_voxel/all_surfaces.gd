
extends Spatial

export(bool) var synchronous_update = true

# Properties used to initialize strategies.
export(float) var focus_depth  = 50.0
export(float) var rebuild_dist = 30.0
export(float) var rescale_dist = 10.0


var _strategy_initialized: bool = false
var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null

var _surfaces: Array = []
var _foliage: Spatial = null




func _ready():
	_enumerate_surfaces()
	
	_strategy_initialized = true
	
	_rebuild_strategy = MarchingCubesRebuildStrategyGd.new()
	var surf: Node = _surfaces[0]
	var radius: float = surf.get_surface_radius()
	
	_rebuild_strategy.init( radius, focus_depth, rescale_dist, rebuild_dist )


func _enumerate_surfaces():
	_surfaces = []
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child(i)
		_surfaces.push_back( ch )
		var ch_qty: int = ch.get_child_count()
		if ch_qty > 0:
			_foliage = ch.get_child( 0 )


func update_source_se3( source_se3: Se3Ref ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var surface: Node = _surfaces[0]

	if not _strategy_initialized:
		var radius: float = surface.get_surface_radius()
		_rebuild_strategy.initialize( radius, scaler )
		_rebuild_strategy.rebuild_angle = 0.2
		_strategy_initialized = true
	
	var view_point_se3: Se3Ref = source_se3.inverse()
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	var r: Vector3 = view_point_se3.r
	DDD.important()
	DDD.print( "r: " + str( r ), 5.0, "aaa" )
	if need_rebuild:
		DDD.print( "need rebuild: " + str( view_point_se3.r ) )
	
	var need_rescale: bool
	if need_rebuild:
		need_rescale = false
	else:
		need_rescale = _rebuild_strategy.need_rescale( view_point_se3 )
	
	if need_rebuild:
		# If needed rebuild, rebuild voxel surface and apply to meshes.
		var data: Array = _rebuild( source_se3, synchronous_update )
		_rebuild_finished( data )
	
	# Else only apply to meshes without applying to the surface.
	if need_rebuild or need_rescale:
		_rescale( source_se3 )
		_rescale_finished()
	
	
	var t: Transform = surface.get_root_se3( source_se3, scaler )
	transform = t




# Rebuild voxel representation and reapply to meshes.
func _rebuild( source_se3: Se3Ref, synch: bool ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	var data: Array = []
	for i in range(qty):
		var surf: Node = _surfaces[i]
		var ret: Array = surf.rebuild_surface( source_se3, scaler, synch )
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



