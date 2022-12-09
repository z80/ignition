
tool
extends Node
class_name FoliageSource

export(Array) var creators = []

### Drag instance creators here.
export(Resource) var drop_creators_here = null setget _set_creator, _get_creator

export(float) var fill_dist      = 120.0
export(float) var fill_node_size = 100.0
var _created_instances: Dictionary = {}
var _rand: IgnRandomGd = null
var _mc_mutex: Mutex = null

# Need to set this one up in _ready().
var _voxel_surface: MarchingCubesDualGd = null

func _init():
	_rand        = IgnRandomGd.new()
	_mc_mutex    = Mutex.new()


func _get_voxel_surface():
	if _voxel_surface == null:
		var parent: Spatial = get_parent()
		_voxel_surface = parent.get_voxel_surface()


func _exit_tree():
	clear()


func _find_planet_surface():
	var n: Node = self
	while true:
		var rf: RefFrameNode = n as RefFrameNode
		if rf != null:
			return rf
		
		n = n.get_parent()
		if n == null:
			break
	
	return null

func async_update_population_prepare( all_surfaces: Node, at_in_source: Vector3, scaler: DistanceScalerBaseRef ):
	#clear()
	_get_voxel_surface()
	
	var force_all: bool = false
	
	var populated_node_paths: Dictionary = {}
	for key in _created_instances:
		populated_node_paths[key] = true
	
	#_voxel_surface.source_se3 = Se3Ref.new()
	var node_indices: Array = _voxel_surface.query_close_nodes( at_in_source, fill_dist, fill_node_size )
	
	var data: AsyncPopulationUpdataData = AsyncPopulationUpdataData.new()
	data.foliage_parent = _find_planet_surface() # This one should be planet.get_rotation()
	data.voxel_surface  = _voxel_surface
	data.voxel_nodes    = []
	data.asset_creators = creators.duplicate()
	data.scaler         = scaler
	data.populated_node_paths = populated_node_paths

	for node_ind in node_indices:
		var voxel_node: MarchingCubesDualNodeGd = _voxel_surface.get_tree_node( node_ind )
		var h_path: String = voxel_node.hierarchy_path()
		
		var already_populated: bool
		if force_all:
			already_populated = false
		
		else:
			already_populated = _created_instances.has( h_path )
		
		if already_populated:
			populated_node_paths.erase( h_path )
		
		else:
			data.voxel_nodes.push_back( voxel_node )
	
	return data



func async_update_view_point_prepare( source_se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	#if _busy_qty != 0:
	#	return false
	
	_get_voxel_surface()
	#_voxel_surface.source_se3 = source_se3

	var data: AsyncViewPointData = AsyncViewPointData.new()
	data.voxel_surface = _voxel_surface
	data.items         = {}
	data.source_se3    = source_se3
	data.scaler        = scaler
	
	for h_path in _created_instances:
		var items: Array = _created_instances[h_path]
		items = items.duplicate()
		#var args: Array = [ items, voxels, source_se3, scaler ]
		data.items[h_path] = items
	
	return data






func clear():
	for key in _created_instances:
		var insts: Array = _created_instances[key]
		for inst in insts:
			if is_instance_valid(inst):
				inst.queue_free()
	
	_created_instances.clear()
	
	return true









func async_populate_node_worker( data: AsyncPopulationUpdataData ):
	var asset_creators: Array = data.asset_creators
	var scaler: DistanceScalerBaseRef = data.scaler
	var voxel_nodes: Array = data.voxel_nodes
	var foliage_parent: RefFrameNode = data.foliage_parent
	
	var all_created_instances: Dictionary = {}
	
	for voxel_node in voxel_nodes:
		_mc_mutex.lock()
		var s: String = voxel_node.hash()
		_mc_mutex.unlock()
		var rand: IgnRandomGd = IgnRandomGd.new()
		rand.seed = s
		
		var h_path: String = voxel_node.hierarchy_path()
		print( "entered node ", h_path )
		
		_mc_mutex.lock()
		var center: Vector3 = voxel_node.center_vector()
		var sz: float       = voxel_node.node_size()
		_mc_mutex.unlock()
		
		var created_instances: Array = []
		
		for creator in asset_creators:
		
			var dist: float = creator.min_distance
			var v: float  = sz / dist
			v = v * v / 2
			var qty: int = int(v)
			
			for i in range(qty):
				var dr: Vector3 = rand.random_vector( sz )
				var c: Vector3 = dr + center
				
				_mc_mutex.lock()
				var ret: Array = voxel_node.intersect_with_segment( c, Vector3.ZERO )
				_mc_mutex.unlock()
				
				var intersects: bool = ret[0]
				if ( not intersects ):
					continue
				
				var at: Vector3   = ret[1]
				var norm: Vector3 = ret[2]
				
				_mc_mutex.lock()
				var se3: Se3Ref = voxel_node.se3_in_point( at )
				_mc_mutex.unlock()
				
				var p: float = creator.probability( se3, norm )
				var rand_p: float = rand.floating_point_closed()
				var create: bool = (rand_p < p)
				if not create:
					continue
				
				var instance: RefFrameNode = creator.create( voxel_node, se3, norm, rand, scaler )
				instance.place( foliage_parent, se3 )
				#print( "created ", instance )
				created_instances.push_back( instance )
		
		all_created_instances[h_path] = created_instances
	
	data.created_instances = all_created_instances
	
	return data


#AsyncPopulationUpdataData
#func _async_populate_node_worker_finished( data: Array ):
func async_populate_node_worker_finished( data: AsyncPopulationUpdataData ):
	#var foliage: Spatial                    = data.foliage
	var all_created_instances: Dictionary   = data.created_instances
	var voxel_nodes: Array                  = data.voxel_nodes
	var populated_node_paths: Dictionary    = data.populated_node_paths
	var foliage_parent: RefFrameNode        = data.foliage_parent
	
	for voxel_node in voxel_nodes:
		var h_path: String = voxel_node.hierarchy_path()
		#print( "callback for node ", h_path )
		
		var created_instances: Array = all_created_instances[h_path]
		for inst in created_instances:
			inst.change_parent( foliage_parent )

		var has: bool = _created_instances.has( h_path )
		if not has:
			_created_instances[h_path] = created_instances
		else:
			var insts: Array = _created_instances[h_path]
			insts += created_instances
			_created_instances[h_path] = insts
	
	
	# Clean up nodes which are not needed anymore.
	for key in populated_node_paths:
		var has: bool = _created_instances.has( key )
		if has:
			var insts: Array = _created_instances[key]
			for inst in insts:
				if is_instance_valid(inst):
					inst.queue_free()
			# Also delete this array from the dictionaty of populated nodes.
			_created_instances.erase( key )





func async_update_view_point_worker( data: AsyncViewPointData ):
	pass




func async_update_view_point_worker_finished( data: AsyncViewPointData ):
	pass
	





func _set_creator( c: Resource ):
	if c != null:
		creators.push_back( c )
	drop_creators_here = null
	property_list_changed_notify()


func _get_creator():
	return null













class AsyncPopulationUpdataData:
	var all_surfaces: Node
	var foliage_parent: Node
	var voxel_surface: MarchingCubesDualGd
	var voxel_nodes: Array
	var asset_creators: Array
	var scaler: DistanceScalerBaseRef
	var populated_node_paths: Dictionary
	
	var created_instances: Dictionary



class AsyncViewPointData:
	var source_se3: Se3Ref
	var scaler: DistanceScalerBaseRef
	var voxel_surface: MarchingCubesDualGd
	var items: Dictionary





