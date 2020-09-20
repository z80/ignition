
extends RigidBody




func set_collision_layer( v: int ):
	collision_layer = v
	collision_mask  = v


func get_collision_layer():
	return collision_layer


