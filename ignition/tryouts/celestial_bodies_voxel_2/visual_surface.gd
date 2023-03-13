tool
extends Node

export(PackedScene) var VisualSurfaceOne = null
export(Resource) var layer_config = null
export(Resource) var surface_source_solid = null
export(Resource) var surface_source_liquid = null
export(Resource) var drop_foliage_source_here = null setget _set_foliage_source, _get_foliage_source

export(Array) var foliage_sources = []

var visual_cells: Dictionary = {}

#var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _ready: bool = false
var _running: bool = false
var _processes_running: int = 0
var _requested_rebuild: bool = false

# This one is used only for bounding boxes generation.
# the same objects inside each cell work independently for 
# parallelization purposes.
var _voxel_surface: MarchingCubesDualGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_ready = false
	
	_initialize_strategy()
	_create_volume_surface()
	# It says it is busy setting up children and 
	# wants it to be called deferred.
	call_deferred( "_create_cells" )


func _initialize_strategy():
	var radius: float  = surface_source_solid.source_radius
	var max_level: int = layer_config.max_level
	
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	_node_size_strategy.radius = radius
	_node_size_strategy.max_level = max_level



func _create_volume_surface():
	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var source: VolumeSourceGd = surface_source_solid.get_source()
	
	var _step: float = voxel_surface.init_min_step( source )
	
	_voxel_surface = voxel_surface



func _create_cells():
	var rotation: RefFrameRotationNode = get_parent()
	for i in range(27):
		var cell: Node = VisualSurfaceOne.instance()
		rotation.add_child( cell )
		visual_cells[i] = cell




func update_source_se3( source_se3: Se3Ref, view_point_se3: Se3Ref ):
	var rebuild_needed: bool = true
	if _running:
		if rebuild_needed:
			_requested_rebuild = true
	
	else:
		rebuild_needed = rebuild_needed or _requested_rebuild
		if rebuild_needed:
			_requested_rebuild = false
			_rebuild_start( source_se3, view_point_se3 )
	




func _rebuild_start( source_se3: Se3Ref, view_point_se3: Se3Ref ):
	# First check if too far.
	var dist: float = view_point_se3.r.length()
	var max_dist: float = surface_source_solid.source_radius * 1.3
	var too_far: bool = dist >= max_dist
	# If it is too far (potentially on a different planet), do nothing.
	if too_far:
		return
	
	var nodes_to_rebuild: Array = _pick_nodes_to_rebuild( view_point_se3 )
	var empty: bool = nodes_to_rebuild.empty()
	if empty:
		return
	_running = true
	
	_processes_running = nodes_to_rebuild.size()
	
	var ids: Array = []
	for data in nodes_to_rebuild:
		var id: String = data.id
		ids.push_back( id )
	#print( "ids: ", ids )
	
	for data in nodes_to_rebuild:
		var node: BoundingNodeGd = data.node
		var visual: Node         = data.visual
		var surface_args = visual.build_surface_prepare( source_se3, view_point_se3, _node_size_strategy, surface_source_solid, surface_source_liquid, foliage_sources )
		surface_args.node = node
		
		var args: Dictionary = {}
		args.node = node
		args.visual = visual
		args.surface_args = surface_args
		
		WorkersPool.push_back_with_arg( self, "_rebuild_process", "_rebuild_finished", args )
		#_rebuild_process( args )
		#_rebuild_finished( args )



func _rebuild_process( args ):
	var node: BoundingNodeGd = args.node
	var visual: Node         = args.visual
	var surface_args         = args.surface_args
	visual.build_surface_process( surface_args )
	return args


func _rebuild_finished( args ):
	var visual: Node         = args.visual
	var surface_args         = args.surface_args
	visual.build_surface_finished( surface_args )
	
	_processes_running -= 1
	if _processes_running == 0:
		_running = false



func _pick_nodes_to_rebuild( view_point_se3: Se3Ref ):
	var sz: float = layer_config.surface_node_size
	#print( "view_point_origin: ", view_point_se3.r )
	var bounding_node: BoundingNodeGd = _voxel_surface.create_bounding_node( view_point_se3, sz )
	var id0: String = bounding_node.get_node_id()
	#print( "central node: ", id0, ", at: ", view_point_se3.r )
	var ids_needed: Array = []
	var nodes_needed: Array = []
	for x in range(3):
		for y in range(3):
			for z in range(3):
				var node: BoundingNodeGd = bounding_node.create_adjacent_node( x-1, y-1, z-1 )
				#var node: BoundingNodeGd = bounding_node.create_adjacent_node( x, y, z )
				var id: String = node.get_node_id()
				nodes_needed.push_back( node )
				ids_needed.push_back( id )
	
	var visuals_free: Array = []
	var ids_to_erase: Array = []
	for id in visual_cells.keys():
		var has: bool = ids_needed.has( id )
		if not has:
			visuals_free.push_back( visual_cells[id] )
			ids_to_erase.push_back( id )
	
	for id in ids_to_erase:
		visual_cells.erase( id )
	
	var visuals_to_be_rebuilt: Array = []
	var qty: int = ids_needed.size()
	var ind: int = 0
	for i in range(qty):
		var id: String = ids_needed[i]
		var has: bool  = visual_cells.has( id )
		if not has:
			var node: BoundingNodeGd = nodes_needed[i]
			var visual: Node = visuals_free[ind]
			ind += 1
			visuals_to_be_rebuilt.push_back( { "node": node, "visual": visual, "id": id } )
			visual_cells[id] = visual
	
	return visuals_to_be_rebuilt



func _set_foliage_source( c: Resource ):
	if c != null:
		foliage_sources.push_back( c )
	drop_foliage_source_here = null
	property_list_changed_notify()


func _get_foliage_source():
	return null





