
extends StaticBody3D

var _shape: CollisionShape3D = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func get_collision_shape():
	if _shape == null:
		_shape = get_node( "CollisionShape3D" )
	
	return _shape



