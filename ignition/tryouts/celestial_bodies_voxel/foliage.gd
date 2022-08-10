
tool
extends Spatial
class_name FoliageSource

export(Array) var creators = []

### Drag instance creators here.
export(Resource) var creator = null setget _set_creator, _get_creator

export(float) var fill_dist      = 120.0
export(float) var fill_node_size = 100.0
var _created_instances: Dictionary = {}
var _rand: IgnRandomGd = null
var _mc_mutex: Mutex = null
var _total_qty_left: int = 0

func _init():
	_rand = IgnRandomGd.new()
	_mc_mutex = Mutex.new()


func _exit_tree():
	clear()


func update( se3: Se3Ref, scaler: DistanceScalerBaseRef, force_all: bool ):
	self.transform = Transform.IDENTITY

	var populated_node_paths: Dictionary = {}
	for key in _created_instances:
		populated_node_paths[key] = true
	
	var parent: Spatial = get_parent()
	var cubes: MarchingCubesDualGd = parent.get_voxel_surface()
	
	var at: Vector3  = se3.r
	var node_indices: Array = cubes.query_close_nodes( at, fill_dist, fill_node_size )
	var total_qty: int = 0
	for node_ind in node_indices:
		var node: MarchingCubesDualNodeGd = cubes.get_tree_node( node_ind )
		var h_path: String = node.hierarchy_path()
		
		var has: bool
		if force_all:
			has = false
		
		else:
			has = _created_instances.has( h_path )
		
		if has:
			populated_node_paths.erase( h_path )
		
		else:
			for creator in creators:
				total_qty += 1
				var args: Array = [self, cubes, node, creator, scaler, populated_node_paths]
				WorkersPool.start_with_args( self, "_populate_node", "_populate_node_callback", args )
	
	_total_qty_left = total_qty
	





func clear():
	if _total_qty_left > 0:
		return false
	
	for key in _created_instances:
		var insts: Array = _created_instances[key]
		for inst in insts:
			if is_instance_valid(inst):
				inst.queue_free()
	
	_created_instances.clear()
	
	return true


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.







func _populate_node( parent: Spatial, cubes: MarchingCubesDualGd, node: MarchingCubesDualNodeGd, creator: Resource, scaler: DistanceScalerBaseRef, populated_node_paths: Dictionary ):
	_mc_mutex.lock()
	var s: String = node.hash()
	_mc_mutex.unlock()
	var rand: IgnRandomGd = IgnRandomGd.new()
	rand.seed = s
	
	var h_path: String = node.hierarchy_path()
	print( "entered node ", h_path )
	
	_mc_mutex.lock()
	var center: Vector3 = node.center_vector( true )
	var sz: float       = node.node_size()
	_mc_mutex.unlock()
	
	var created_instances: Array = []
	
	var dist: float = creator.min_distance
	var v: float  = sz / dist
	v = v * v * 50
	var qty: int = int(v)
	
	for i in range(qty):
		var dr: Vector3 = rand.random_vector( sz )
		var c: Vector3 = dr + center
		
		_mc_mutex.lock()
		var ret: Array = node.intersect_with_segment( c, Vector3.ZERO, true )
		_mc_mutex.unlock()
		
		var intersects: bool = ret[0]
		if ( not intersects ):
			continue
		
		var at: Vector3   = ret[1]
		var norm: Vector3 = ret[2]
		
		_mc_mutex.lock()
		var se3: Se3Ref = node.se3_in_point( at, true )
		_mc_mutex.unlock()
		
		var p: float = creator.probability( se3, norm )
		var rand_p: float = rand.floating_point_closed()
		var create: bool = (rand_p < p)
		if not create:
			continue
		
		var instance: Spatial = creator.create( node, se3, rand, scaler )
		#print( "created ", instance )
		created_instances.push_back( instance )
	
	print( "left node ", h_path )
	return [ parent, created_instances, node, populated_node_paths ]


func _populate_node_callback( data: Array ):
	var parent: Spatial                  = data[0]
	var created_instances: Array         = data[1]
	var node: MarchingCubesDualNodeGd    = data[2]
	var populated_node_paths: Dictionary = data[3]
	
	var h_path: String = node.hierarchy_path()
	print( "callback for node ", h_path )
	
	for inst in created_instances:
		var t: Transform = inst.transform
		parent.add_child( inst )
		inst.transform = t

	var has: bool = _created_instances.has( h_path )
	if not has:
		_created_instances[h_path] = created_instances
	else:
		var insts: Array = _created_instances[h_path]
		insts += created_instances
		_created_instances[h_path] = insts
	
	_total_qty_left -= 1
	if _total_qty_left <= 0:
		for key in populated_node_paths:
			has = _created_instances.has( key )
			if has:
				var insts: Array = _created_instances[key]
				for inst in insts:
					if is_instance_valid(inst):
						inst.queue_free()




func _set_creator( c: Resource ):
	if c != null:
		creators.push_back( c )
	creator = null
	property_list_changed_notify()


func _get_creator():
	return null
