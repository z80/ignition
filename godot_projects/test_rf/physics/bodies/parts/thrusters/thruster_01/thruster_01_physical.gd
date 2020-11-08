
extends RigidBody

const MIN_THRUST: float = 10.0
const MAX_THRUST: float = 20.0

export(bool) var enabled = false setget _set_enabled
export(float) var throttle = 0.0 setget _set_throttle

var user_input: Dictionary = {}


func _set_enabled( en ):
	enabled = en


func _set_throttle( th ):
	throttle = th


func _integrate_forces( state ):
	if enabled and (throttle > 0.0):
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







