@tool
extends Node

@export var CollisionSurfaceOne: PackedScene = null
@export var layer_config: Resource   = null

var _collision_cells: Dictionary = {}

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


func _exit_tree():
	var delete: bool = is_queued_for_deletion()
	if delete:
		for id in _collision_cells:
			var cell: RefFrameNode = _collision_cells[id]
			cell.queue_free()
		_collision_cells.clear()


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
	var full: bool = not _collision_cells.is_empty()
	if full:
		return
	
	var ref_frame_physics: RefFramePhysics = get_parent()
	for i in range(27):
		var cell: Node = CollisionSurfaceOne.instantiate()
		ref_frame_physics.add_child( cell )
		_collision_cells[i] = cell


func _create_cell():
	var ref_frame_physics: RefFramePhysics = get_parent();
	var cell: Node = CollisionSurfaceOne.instantiate()
	ref_frame_physics.add_child( cell )
	return cell




# This thing should be called externally by a rotational part of a planet.
func rebuild_surface( surface_source: Resource, foliage_sources: Array, synchronous: bool = true ):
	var ref_frame_physics: RefFramePhysics = get_parent()
	var rotation_node: RefFrameRotationNode = ref_frame_physics.get_parent()
	if rotation_node == null:
		return
	
	var view_point_se3: Se3Ref = ref_frame_physics.relative_to( rotation_node )
	
	var dist: float = view_point_se3.r.length()
	var max_dist: float = surface_source.bounding_radius * 1.3
	var too_far: bool = dist >= max_dist
	# If it is too far (potentially on a different planet), do nothing.
	if too_far:
		return
	
	var source_se3: Se3Ref = view_point_se3.inverse()
	
	var view_point_se3s: Array = []
	var bodies: Array = ref_frame_physics.root_most_child_bodies()
	var qty: int = bodies.size()
	for i in range(qty):
		var body: RefFrameNode = bodies[i]
		var body_se3: Se3Ref = body.relative_to( rotation_node )
		view_point_se3s.push_back( body_se3 )
	
	_initialize( surface_source )
	
	_rebuild_start( surface_source, foliage_sources, source_se3, view_point_se3s, synchronous )


# When leaving the rotation node need to remove collision surface.
func remove_surface():
	if _running:
		return
	
	for cell in _collision_cells:
		cell.remove_surface()


# Should be called by a ref. frame which has created this one.
func clone_surface( other_surface ):
	if _running:
		return
	
	_create_cells()
	var own_cells: Array = []
	for key in _collision_cells:
		var cell: Node = _collision_cells[key]
		own_cells.push_back( cell )
	_collision_cells.clear()
	
	var ref_frame_physics: RefFrameNonInertialNode = get_parent()
	
	var other_cells: Dictionary = other_surface.collision_cells
	var ind: int = 0
	for key in other_cells:
		var other_cell: RefFrameNode = other_cells[key]
		var own_cell: RefFrameNode = own_cells[ind]
		
		var se3: Se3Ref = other_cell.relative_to( ref_frame_physics )
		own_cell.set_se3( se3 )
		own_cell.clone_surface( other_cell )
		
		_collision_cells[key] = own_cell
		
		ind += 1
		



func _rebuild_start( surface_source: Resource, foliage_sources: Array, source_se3: Se3Ref, view_point_se3s: Array, synchronous: bool = false ):
	var nodes_to_rebuild: Array = _pick_nodes_to_rebuild( view_point_se3s )
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
	
	var view_point_se3: Se3Ref = view_point_se3s[0]
	
	for data in nodes_to_rebuild:
		var node: BoundingNodeGd = data.node
		var collision: Node      = data.collision
		var surface_args         = collision.build_surface_prepare( source_se3, view_point_se3, _node_size_strategy, surface_source, foliage_sources )
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



func _pick_nodes_to_rebuild( view_point_se3s: Array ):
	var sz: float = layer_config.surface_node_size
	#print( "view_point_origin: ", view_point_se3.r )
	var id_node_needed: Dictionary = {}
	for view_point_se3 in view_point_se3s:
		var bounding_node: BoundingNodeGd = _voxel_surface.create_bounding_node( view_point_se3, sz )
		#print( "central node: ", id0, ", at: ", view_point_se3.r )
		for x in range(3):
			for y in range(3):
				for z in range(3):
					var dx: int = x-1
					var dy: int = y-1
					var dz: int = z-1
					var id: String = bounding_node.get_adjacent_node_id( dx, dy, dz )
					var has: bool = id_node_needed.has( id )
					if has:
						continue
					
					var node: BoundingNodeGd = bounding_node.create_adjacent_node( dx, dy, dz )
					id_node_needed[id] = node
	
	var ids_to_erase: Array = []
	for id in _collision_cells.keys():
		var has: bool = id_node_needed.has( id )
		if not has:
			_collision_cells[id].queue_free()
			ids_to_erase.push_back( id )
	
	for id in ids_to_erase:
		_collision_cells.erase( id )
	
	var collisions_to_be_rebuilt: Array = []
	var qty: int = id_node_needed.size()
	var ind: int = 0
	for id in id_node_needed:
		var has: bool  = _collision_cells.has( id )
		if not has:
			var node: BoundingNodeGd = id_node_needed[id]
			var collision: Node = _create_cell()
			ind += 1
			collisions_to_be_rebuilt.push_back( { "node": node, "collision": collision, "id": id } )
			_collision_cells[id] = collision
	
	return collisions_to_be_rebuilt



func get_bounding_nodes():
	if _running:
		return []
	
	var nodes: Array = []
	for id in _collision_cells:
		var collision: Node = _collision_cells[id]
		var node: BoundingNodeGd = collision.bounding_node
		nodes.push_back( node )
	
	return nodes


func get_collision_cells():
	if _running:
		return []
	
	var cells: Array = []
	for id in _collision_cells:
		var cell: Node = _collision_cells[id]
		cells.push_back( cell )
	
	return cells
	


func get_bounding_node_clusters( other_bounding_nodes = null ):
	if _running:
		return []
	
	var nodes: Array = []
	for id in _collision_cells:
		var collision: Node = _collision_cells[id]
		var node: BoundingNodeGd = collision.bounding_node
		if node == null:
			continue
		nodes.push_back( node )
	
	if other_bounding_nodes != null:
		var other_qty: int = other_bounding_nodes.size()
		for i in range(other_qty):
			var node: BoundingNodeGd = other_bounding_nodes[i]
			if node == null:
				continue
			nodes.push_back( node )
	
	var clusters: Array = BoundingNodeGd.split_into_clusters( nodes )
	return clusters



func move_cells_to_other_ref_frame( nodes: Array, destination_ref_frame: RefFrameNode ):
	var other_collision_surface: Node = destination_ref_frame.get_collision_surface()
	var qty: int = nodes.size()
	for i in range(qty):
		var node: BoundingNodeGd = nodes[i]
		var id: String = node.get_node_id()
		var cell: RefFrameNode = _collision_cells[id]
		_collision_cells.erase( id )
		cell.change_parent( destination_ref_frame, false )
		other_collision_surface._collision_cells[id] = cell
		



func get_surface():
	return _voxel_surface


func is_running():
	return _running








