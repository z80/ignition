
extends RigidBody


export(Vector3) var thrust = Vector3.ZERO setget _set_thrust

var user_input: Dictionary = {}


func _set_thrust( th ):
	thrust = th
	sleeping = false


func _integrate_forces( state ):
	var l: float = thrust.length()
	if l > 0.0:
		state.add_central_force( thrust );


func set_collision_layer( v: int ):
	collision_layer = v
	collision_mask  = v


func get_collision_layer():
	return collision_layer





#func _physics_process(_delta):
#	var v: Vector3 = linear_velocity
#	print( "thruster v: ", v )



