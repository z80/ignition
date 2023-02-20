
extends StaticBody

var _shape: CollisionShape = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func get_collision_shape():
	if _shape == null:
		_shape = get_node( "CollisionShape" )
	
	return _shape



