
tool
extends Node

export(Array) var creators = []

### Drag instance creators here.
export(Resource) var creator = null setget _set_creator, _get_creator

export(float) var fill_dist      = 120.0
export(float) var fill_node_size = 100.0
var _created_instances: Dictionary = {}
var _rand: IgnRandomGd = null


func _init():
	_rand = IgnRandomGd.new()


func _exit_tree():
	clear()


func update( parent: Spatial, cubes: MarchingCubesDualGd, se3: Se3Ref, scaler: DistanceScalerBaseRef ):
	
	var all_node_keys: Dictionary = {}
	for key in _created_instances:
		all_node_keys[key] = true
	
	var at: Vector3  = se3.r
	var node_indices: Array = cubes.query_close_nodes( at, fill_dist, fill_node_size )
	for node_ind in node_indices:
		var node: MarchingCubesDualNodeGd = cubes.get_tree_node( node_ind )
		var h_path: String = node.hierarchy_path()
		var has: bool = _created_instances.has( h_path )
		if not has:
			for creator in creators:
				var created_instances: Array = _populate_node( parent, cubes, node, creator, scaler )
				has = _created_instances.has( h_path )
				if not has:
					_created_instances[h_path] = created_instances
				else:
					var insts: Array = _created_instances[h_path]
					insts += created_instances
					_created_instances[h_path] = insts
		
		# Remove from the list of all populated nodes.
		# There should be only node paths which should be eliminated in the end.
		has = all_node_keys.has( h_path )
		if has:
			all_node_keys.erase( h_path )
	
	# Remove node content which are now beyond the range and 
	# should not be populated anymore.
	for key in all_node_keys:
		var has: bool = _created_instances.has( key )
		if has:
			var insts: Array = _created_instances[key]
			for inst in insts:
				if is_instance_valid(inst):
					inst.queue_free()




func clear():
	for key in _created_instances:
		var insts: Array = _created_instances[key]
		for inst in insts:
			if is_instance_valid(inst):
				inst.queue_free()
	
	_created_instances.clear()


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _populate_node( parent: Spatial, cubes: MarchingCubesDualGd, node: MarchingCubesDualNodeGd, creator: Resource, scaler: DistanceScalerBaseRef ):
	var s: String = node.hash()
	_rand.seed = s
	
	var center: Vector3 = node.center_vector( true )
	var sz: float       = node.node_size()
	
	var created_instances: Array = []
	
	var dist: float = creator.min_distance
	var v: float  = sz / dist
	v = v * v * 50
	var qty: int = int(v)
	
	for i in range(qty):
		var dr: Vector3 = _rand.random_vector( sz )
		var c: Vector3 = dr + center
		var ret: Array = node.intersect_with_segment( c, Vector3.ZERO, true )
		var intersects: bool = ret[0]
		if ( not intersects ):
			continue
		
		var at: Vector3   = ret[1]
		var norm: Vector3 = ret[2]
		
		var se3: Se3Ref = node.se3_in_point( at, true )
		
		var p: float = creator.probability( se3, norm )
		var rand_p: float = _rand.floating_point_closed()
		var create: bool = (rand_p < p)
		if not create:
			continue
		
		var instance: Spatial = creator.create( parent, node, se3, _rand, scaler )
		created_instances.push_back( instance )
	
	return created_instances




func _set_creator( c: Resource ):
	if c != null:
		creators.push_back( c )
	creator = null
	property_list_changed_notify()


func _get_creator():
	return null
