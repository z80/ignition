
tool
extends Spatial

export(Array) var fillers = []
export(Resource) var filler = null setget _set_filler, _get_filler

export(float) var fill_dist      = 120.0
export(float) var fill_node_size = 100.0
var _currently_filled_nodes: Array = []


func update( cubes: MarchingCubesDualGd, se3: Se3Ref ):
	var at: Vector3  = se3.r
	var nodes: Array = cubes.query_close_nodes( at, fill_dist, fill_node_size )



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass



func _fill_node( node: MarchingCubesDualNodeGd, filler: Resource ):
	var c: Vector3 = node.center_vector( true )
	var sz: float  = node.size()




func _set_filler( f: Resource ):
	if f != null:
		fillers.push_back( f )
	filler = null
	property_list_changed_notify()


func _get_filler():
	return null
