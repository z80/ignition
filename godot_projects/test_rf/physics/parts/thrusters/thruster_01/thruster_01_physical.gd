
extends RigidBody

const MIN_THRUST: float = 1.0
const MAX_THRUST: float = 5.0

export(bool) var on = false setget _set_on
export(float) var throttle = 1.0 setget _set_throttle

var user_input: Dictionary = {}


func _set_on( en ):
	on = en
	sleeping = false


func _set_throttle( th ):
	throttle = th
	sleeping = false


func _integrate_forces( state ):
	if on and (throttle > 0.0):
		var th: float = MIN_THRUST + (MAX_THRUST - MIN_THRUST) * throttle
		var t: Transform = self.transform
		var b: Basis = t.basis
		var f: Vector3 = b.xform( Vector3.UP * th )
		state.add_central_force( f );


func set_collision_layer( v: int ):
	collision_layer = v
	collision_mask  = v


func get_collision_layer():
	return collision_layer





#func _physics_process(_delta):
#	var v: Vector3 = linear_velocity
#	print( "thruster v: ", v )



