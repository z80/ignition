
extends RigidBody

const MAX_TORQUE: float   = 50.0
const ANG_VEL: float      = 1.0
const GAIN_ANGULAR: float = 100.0

const rotation_abolute: bool = false


var user_input: Dictionary = {}

var rotation_lock: bool = false


func _integrate_forces( state ):
	ang_vel_control( state )


func set_collision_layer( v: int ):
	collision_layer = v
	collision_mask  = v


func get_collision_layer():
	return collision_layer





func ang_vel_control( state ):
	var dt: float = state.step
	
	var w: Vector3 = Vector3.ZERO
	
	if not user_input.empty():
		print( "wheels physical input: ", user_input )
	
	var i: bool = user_input.has( "ui_i" )
	var k: bool = user_input.has( "ui_k" )
	var j: bool = user_input.has( "ui_j" )
	var l: bool = user_input.has( "ui_l" )
	var u: bool = user_input.has( "ui_u" )
	var o: bool = user_input.has( "ui_o" )

	if i:
		w += Vector3.RIGHT
	if k:
		w += Vector3.LEFT
	if j:
		w += Vector3.UP
	if l:
		w += Vector3.DOWN
	if u:
		w += Vector3.BACK
	if o:
		w += Vector3.FORWARD
	
	if w.length_squared() > 0.0:
		w = w.normalized()
	
	# If no control and no orientation lock, 
	# don't force angular velocity.
	elif not rotation_lock:
		return
	
	# Adjustment q.
	var t: Transform = self.transform
	var q: Quat = t.basis
	
	var wanted_w = w
	if not rotation_abolute:
		wanted_w = q.xform( wanted_w )
	wanted_w *= ANG_VEL
	var current_w: Vector3 = state.angular_velocity
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * GAIN_ANGULAR * dw
	var L = torque.length()

	if L > MAX_TORQUE:
		torque = torque * (MAX_TORQUE / L)
	
	state.add_torque( torque )

