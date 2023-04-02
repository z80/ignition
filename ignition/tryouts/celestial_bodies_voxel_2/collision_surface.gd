@tool
extends Node

@export var CollisionSurfaceOne: PackedScene = null
@export var layer_config: Resource   = null

var collision_cells: Dictionary = {}

#var _node_size_strategy: VolumeNodeSizeStrategyGd = null

var _running: bool = false
var _processes_running: int = 0

# This one is used only for bounding boxes generation.
# the same objects inside each cell work independently for 
# parallelization purposes.
var _voxel_surface: MarchingCubesDualGd = null
var _node_size_strategy: VolumeNodeSizeStrategyGd = null


# Called when the node enters the scene tree for the first time.
func _ready():
	pass



func _initialize( surface_source: Resource ):
	_initialize_strategy( surface_source )
	_create_volume_surface( surface_source )
	_create_cells()



func _initialize_strategy( surface_source: Resource ):
	var radius: float  = surface_source.source_radius
	var max_level: int = layer_config.max_level
	
	if _node_size_strategy == null:
		_node_size_strategy = VolumeNodeSizeStrategyGd.new()
	
	_node_size_strategy.radius = radius
	_node_size_strategy.max_level = max_level



func _create_volume_surface( surface_source: Resource ):
	if _voxel_surface == null:
		_voxel_surface = MarchingCubesDualGd.new()
		_voxel_surface.max_nodes_qty   = 20000000
		_voxel_surface.split_precision = 0.01
	
	var source: VolumeSourceGd = surface_source.get_source_solid()
	var _step: float = _voxel_surface.init_min_step( source )




func _create_cells():
	var full: bool = not collision_cells.is_empty()
	if full:
		return
	
	var ref_frame_physics: RefFramePhysics = get_parent();
	for i in range(27):
		var cell: Node = CollisionSurfaceOne.instantiate()
		ref_frame_physics.add_child( cell )
		collision_cells[i] = cell





# This thing should be called externally by a rotational part of a planet.
func rebuild_surface( surface_source: Resource, synchronous: bool = true ):
	var ref_frame_physics: RefFramePhysics = get_parent()
	var rotation_node: RefFrameRotationNode = ref_frame_physics.get_parent()
	if rotation_node == null:
		return
	
	var view_point_se3: Se3Ref = ref_frame_physics.relative_to( rotation_node )
	var source_se3: Se3Ref = view_point_se3.inverse()
	
	_initialize( surface_source )
	
	_rebuild_start(Callable(surface_source, source_se3).bind(view_point_se3), synchronous)


# When leaving the rotation node need to remove collision surface.
func remove_surface():
	if _running:
		return
	
	for cell in collision_cells:
		cell.remove_surface()


# Should be called by a ref. frame which has created this one.
func clone_surface( other_surface ):
	if _running:
		return
	
	_create_cells()
	var own_cells: Array = []
	for key in collision_cells:
		var cell: Node = collision_cells[key]
		own_cells.push_back( cell )
	collision_cells.clear()
	
	var ref_frame_physics: RefFrameNonInertialNode = get_parent()
	
	var other_cells: Dictionary = other_surface.collision_cells
	var ind: int = 0
	for key in other_cells:
		var other_cell: RefFrameNode = other_cells[key]
		var own_cell: RefFrameNode = own_cells[ind]
		
		var se3: Se3Ref = other_cell.relative_to( ref_frame_physics )
		own_cell.set_se3( se3 )
		own_cell.clone_surface( other_cell )
		
		collision_cells[key] = own_cell
		
		ind += 1
		



func _rebuild_start( surface_source: Resource, source_se3: Se3Ref, view_point_se3: Se3Ref, synchronous: bool = false ):
	var nodes_to_rebuild: Array = _pick_nodes_to_rebuild( view_point_se3 )
	var empty: bool = nodes_to_rebuild.is_empty()
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
		var surface_args         = collision.build_surface_prepare( source_se3, view_point_se3, _node_size_strategy, surface_source )
		surface_args.node        = node
		
		var args: Dictionary = {}
		args.node = node
		args.collision      = collision
		args.surface_args   = surface_args
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








