
extends RigidBody3D


@export var thrust: Vector3 = Vector3.ZERO: set = _set_thrust

var user_input: Dictionary = {}


func _set_thrust( th ):
	thrust = th
	sleeping = false


func _integrate_forces( state ):
	var l: float = thrust.length()
	DDD.important()
	DDD.print( "thrust_used:     " + str(l), -1.0, "thrust_used" )
	if l > 0.0:
		state.apply_central_force( thrust );





