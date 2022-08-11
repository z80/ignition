
extends Spatial

export(bool) var synchronous_update = true

var _strategy_initialized: bool = false
var _rebuild_strategy: MarchingCubesSphericalRebuildStrategyGd = null

var _surfaces: Array = []
var _foliage: Spatial = null


func _init():
	_strategy_initialized = false
	_rebuild_strategy = MarchingCubesSphericalRebuildStrategyGd.new()


func _ready():
	_enumerate_surfaces()


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
	
	var camera_se3: Se3Ref = source_se3.inverse()
	var need_rebuild: bool = _rebuild_strategy.need_rebuild( camera_se3 )
	var r: Vector3 = camera_se3.r
	DDD.print( "r: " + str( r ), 5.0, "aaa" )
	if need_rebuild:
		DDD.print( "need rebuild: " + str( need_rebuild ) )
	
	if need_rebuild:
		# If needed rebuild, rebuild voxel surface and apply to meshes.
		_rebuild( source_se3, synchronous_update )
		synchronous_update = false
	
	else:
		# Else only apply to meshes without applying to the surface.
		#_update_view_point( source_se3 )
		pass
	
	var t: Transform = surface.apply_root_se3( source_se3, scaler )
	transform = t




# Rebuild voxel representation and reapply to meshes.
func _rebuild( source_se3: Se3Ref, synch: bool ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	for i in range(qty):
		var surf: Node = _surfaces[i]
		surf.update( source_se3, scaler, synch )
	
	var point_se3: Se3Ref = source_se3.inverse()
	_foliage.update( point_se3, scaler, true )


# Just re-apply to meshes without rebuilding voxel surface.
func _update_view_point( source_se3: Se3Ref ):
	var scaler: DistanceScalerBaseRef = PhysicsManager.distance_scaler
	var qty: int = _surfaces.size()
	for i in range(qty):
		var surf: Node = _surfaces[i]
		surf.apply_meshes( source_se3, scaler )



