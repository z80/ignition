tool
extends Node

export(PackedScene) var CollisionSurfaceOne = null
export(Resource) var layer_config   = null
export(Resource) var surface_source = null

var collision_cells: Dictionary = {}

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
	var radius: float  = surface_source.source_radius
	var max_level: int = layer_config.max_level
	
	_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	_node_size_strategy.radius = radius
	_node_size_strategy.max_level = max_level



func _create_volume_surface():
	var voxel_surface: MarchingCubesDualGd = MarchingCubesDualGd.new()
	voxel_surface.max_nodes_qty   = 20000000
	voxel_surface.split_precision = 0.01
	
	var source: VolumeSourceGd = surface_source.get_source()
	
	var _step: float = voxel_surface.init_min_step( source )
	
	_voxel_surface = voxel_surface



func _create_cells():
	var ref_frame_physics: RefFramePhysics = get_parent();
	for i in range(27):
		var cell: Node = CollisionSurfaceOne.instance()
		ref_frame_physics.add_child( cell )
		collision_cells[i] = cell
	
	rebuild( true )




func _create_size_strategy( surface_source ):
	var radius: float  = surface_source.source_radius
	var max_level: int = layer_config.max_level
	
	var node_size_strategy: VolumeNodeSizeStrategyGd = VolumeNodeSizeStrategyGd.new()
	node_size_strategy.radius    = radius
	node_size_strategy.max_level = max_level
	
	return node_size_strategy


func _parent_jumped():
	rebuild( false )


func rebuild( synchronous: bool = true ):
	var rebuild_needed: bool = true
	if _running:
		if rebuild_needed:
			_requested_rebuild = true
	
	else:
		var ref_frame_physics: RefFramePhysics = get_parent()
		var rotation_node: RefFrameRotationNode = ref_frame_physics.get_parent()
		if rotation_node == null:
			return
		
		var view_point_se3: Se3Ref = ref_frame_physics.relative_to( rotation_node )
		var source_se3: Se3Ref = view_point_se3.inverse()
		
		rebuild_needed = rebuild_needed or _requested_rebuild
		if rebuild_needed:
			_requested_rebuild = false
			_rebuild_start( source_se3, view_point_se3, synchronous )
	




func _rebuild_start( source_se3: Se3Ref, view_point_se3: Se3Ref, synchronous: bool = false ):
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
		var collision: Node      = data.collision
		var surface_args = collision.build_surface_prepare( source_se3, view_point_se3, _node_size_strategy, surface_source )
		surface_args.node = node
		
		var args: Dictionary = {}
		args.node = node
		args.collision    = collision
		args.surface_args = surface_args
		args.view_point_se3 = view_point_se3
		
		if synchronous:
			_rebuild_process( args )
			_rebuild_finished( args )
		
		else:
			WorkersPool.push_back_with_arg( self, "_rebuild_process", "_rebuild_finished", args )



func _rebuild_process( args ):
	var node: BoundingNodeGd    = args.node
	var collision: RefFrameNode = args.collision
	var surface_args            = args.surface_args
	collision.build_surface_process( surface_args )
	return args


func _rebuild_finished( args ):
	var collision: RefFrameNode = args.collision
	var view_point_se3: Se3Ref  = args.view_point_se3
	var surface_args            = args.surface_args
	collision.build_surface_finished( surface_args )
	
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
	
	var collisions_free: Array = []
	var ids_to_erase: Array = []
	for id in collision_cells.keys():
		var has: bool = ids_needed.has( id )
		if not has:
			collisions_free.push_back( collision_cells[id] )
			ids_to_erase.push_back( id )
	
	for id in ids_to_erase:
		collision_cells.erase( id )
	
	var collisions_to_be_rebuilt: Array = []
	var qty: int = ids_needed.size()
	var ind: int = 0
	for i in range(qty):
		var id: String = ids_needed[i]
		var has: bool  = collision_cells.has( id )
		if not has:
			var node: BoundingNodeGd = nodes_needed[i]
			var collision: Node = collisions_free[ind]
			ind += 1
			collisions_to_be_rebuilt.push_back( { "node": node, "collision": collision, "id": id } )
			collision_cells[id] = collision
	
	return collisions_to_be_rebuilt








