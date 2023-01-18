
extends Spatial

export(Resource) var layer_config = null
export(Resource) var surface_source_solid = null
export(Resource) var surface_source_liquid = null

var surface: MeshInstance = null
var liquid: MeshInstance = null

var _rebuild_strategy: MarchingCubesRebuildStrategyGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	var root: Spatial = get_node( "Visual" )
	surface = root.surface
	liquid  = root.liquid
	
	_initialize_rebuild_strategy()


func _initialize_rebuild_strategy():
	var radius: float      = surface_source_solid.source_radius
	
	var focus_depth: float = radius * layer_config.min_relative_focal_depth
	var rebuild_dist: float = radius * layer_config.relative_rebuild_dist
	
	_rebuild_strategy = MarchingCubesRebuildStrategyGd.new()
	_rebuild_strategy.radius = radius
	_rebuild_strategy.height = focus_depth
	_rebuild_strategy.rebuild_dist = rebuild_dist
	
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	_node_size_strategy.radius = radius
	_node_size_strategy.max_level = 6


func update_source_se3( view_point_se3: Se3Ref ):
	var rebuild_needed: bool = _rebuild_strategy.need_rebuild( view_point_se3 )
	if not rebuild_needed:
		return
	
	_rebuild_start()



func _rebuild_start():
	_node_size_strategy.focal_point = _rebuild_strategy.get_focal_point_rebuild()


func _rebuild_process( args ):
	pass


func _rebuild_finish( args ):
	pass

