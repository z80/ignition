
tool
extends Spatial

export(Array) var creators = []
export(Resource) var creator = null setget _set_creator, _get_creator

export(float) var fill_dist      = 120.0
export(float) var fill_node_size = 100.0
var _currently_populated_nodes: Array = []
var _created_instances: Array = []

func update( cubes: MarchingCubesDualGd, se3: Se3Ref ):
	var at: Vector3  = se3.r
	var nodes: Array = cubes.query_close_nodes( at, fill_dist, fill_node_size )




func clear():
	for inst in _created_instances:
		inst.queue_free()
	
	_created_instances.clear()


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _fill_node( parent: Spatial, cubes: MarchingCubesDualGd, node: MarchingCubesDualNodeGd, creator: Resource, scaler: DistanceScalerBaseRef ):
	var center: Vector3 = node.center_vector( true )
	var sz: float       = node.node_size()
	
	
	var dist: float = creator.min_distance
	var v: float  = sz / dist
	v = v * v
	var qty: int = int(v)
	
	for i in range(qty):
		var dx: float = (randf() - 0.5) * sz
		var dy: float = (randf() - 0.5) * sz
		var dz: float = (randf() - 0.5) * sz
		var c: Vector3 = Vector3( dx, dy, dz ) + center
		var ret: Array = node.intersect_with_segment( c, Vector3.ZERO, true )
		var intersects: bool = ret[0]
		if ( not intersects ):
			continue
		
		var at: Vector3   = ret[1]
		var norm: Vector3 = ret[2]
		
		var se3: Se3Ref = node.se3_in_point( at, true )
		
		var p: float = creator.probability( se3, norm )
		var rand_p: float = randf()
		var create: bool = (rand_p < p)
		if not create:
			continue
		
		var instance: Spatial = creator.create( parent, node, se3, scaler )




func _set_creator( c: Resource ):
	if c != null:
		creators.push_back( c )
	creator = null
	property_list_changed_notify()


func _get_creator():
	return null
