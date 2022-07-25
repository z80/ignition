
extends Spatial

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


