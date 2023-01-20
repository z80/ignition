
extends Spatial

export(PackedScene) var VisualSurfaceOne = null
export(Resource) var layer_config = null
export(Resource) var surface_source_solid = null
export(Resource) var surface_source_liquid = null
export(Resource) var foliage_source = null

var visual_cells: Dictionary = {}

#var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _center_se3: Se3Ref = null

var _ready: bool = false
var _running: bool = false
var _requested_rebuild: bool = false

# Called when the node enters the scene tree for the first time.
func _ready():
	var root: Spatial = get_node( "Visual" )
	#surface = root.surface
	#liquid  = root.liquid
	
	_center_se3 = Se3Ref.new()
	
	_ready = false
	
	#_initialize_strategies()
	_create_cells()


func _create_cells():
	var rotation: RefFrameRotationNode = get_parent()
	for i in range(27):
		var cell: Node = VisualSurfaceOne.instance()
		rotation.add_child( cell.visual )
		visual_cells[i] = cell


#func _initialize_strategies():
#	var radius: float      = surface_source_solid.source_radius
#
#	var focus_depth: float = radius * layer_config.min_relative_focal_depth
#	var rebuild_dist: float = radius * layer_config.relative_rebuild_dist
#	var max_level: int = layer_config.max_level
#
#	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
#	_node_size_strategy.radius = radius
#	_node_size_strategy.max_level = max_level


func update_source_se3( source_se3: Se3Ref, view_point_se3: Se3Ref ):
	var rebuild_needed: bool = true
	if _running:
		if rebuild_needed:
			_requested_rebuild = true
	
	else:
		rebuild_needed = rebuild_needed or _requested_rebuild
		if rebuild_needed:
			_rebuild_start()
	




func _rebuild_start():
	_running = true
	_requested_rebuild = false
	
	var source: VolumeSourceGd = surface_source_solid.get_source()
	var scale: float = 1.0 / layer_config.scale_divider
	var args: Dictionary = { "source": source, "scale": scale }
	
	#WorkersPool.push_back_with_arg( self, "_rebuild_process", "_rebuild_finished", args )
	var ret = _rebuild_process( args )
	_rebuild_finished( ret )


func _rebuild_process( args ):
	var source: VolumeSourceGd = args.source
	var scale: float = args.scale
	
	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var ok: bool = voxel_surface.subdivide_source_all( source, null )
	voxel_surface.precompute_scaled_values( _center_se3, _center_se3, 0, scale )
	
	var ret: Dictionary = { "voxel_surface": voxel_surface }
	return ret


func _rebuild_finished( args ):
	_ready = true
	_running = false
	var voxel_surface: MarchingCubesDualGd = args.voxel_surface
	var qty: int = voxel_surface.get_nodes_qty()
	print( "rebuild done, nodes qty: ", qty )
	#voxel_surface.apply_to_mesh_only( surface )
	#surface.material_override = surface_source_solid.materials[0]










class Cell:
	var visual: Spatial
	var bounding_box: BoundingNodeGd





