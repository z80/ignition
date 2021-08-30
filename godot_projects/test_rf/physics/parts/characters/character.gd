
extends Part
class_name Character

export(float) var speed     = 3.0
export(float) var gain      = 50.5
export(float) var max_force = 5.0


export(float) var ang_vel        = 1.0
export(float) var max_ang_vel    = 2.0
export(float) var gain_angular   = 100.0
export(float) var gain_d_angular = 100.0
export(float) var max_torque     = 5.0

export(bool) var translation_abolute = false
export(bool) var rotation_abolute    = false

# Amount of force projected onto this direction is zeroed.
export(bool) var translation_do_ignore_direction = true
export(Vector3) var translation_ignore_direction = Vector3.UP


# If it is flying in space.
export(bool) var free_floating = false
# If it should resist tipping over.
export(bool) var preserve_vertical = true

var target_q: Quat    = Quat.IDENTITY
var local_up: Vector3 = Vector3.UP

var print_period: float  = 0.1
var print_elapsed: float = 0.0

var _input: Dictionary = {}


# Need this mode in order to get in/out a habitat module.
enum BoardingMode {
	OUTSIDE=0, 
	INSIDE=1
}

export(BoardingMode) var boarding_mode = BoardingMode.OUTSIDE setget set_boarding_mode, get_boarding_mode


func set_boarding_mode( new_mode ):
	if new_mode == BoardingMode.INSIDE:
		set_boarding_mode_inside()
	elif new_mode == BoardingMode.OUTSIDE:
		set_boarding_mode_outside()


func set_boarding_mode_inside():
	remove_physical()
	_visual.visible = false
	body_state = BodyState.KINEMATIC
	boarding_mode = BoardingMode.INSIDE


func set_boarding_mode_outside():
	_visual.visible = true
	body_state = BodyState.DYNAMIC
	update_physics_from_state()
	boarding_mode = BoardingMode.OUTSIDE


func get_boarding_mode():
	return boarding_mode


func init():
	.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


# Copying user input into a local variable. So it can be processed 
# when it wants to do so.
func process_user_input_group( input: Dictionary ):
	_input = input.duplicate( true )


func process_inner( delta ):
	.process_inner( delta )
	# Update visual animation state.
	if (_physical != null) and (_visual != null):
		# This is for visualizing walk.
		var ret: Array = get_speed_normalized( _physical )
		var s: float   = ret[0]
		var v: Vector3 = ret[1]
		_visual.set_speed_normalized( s, v )



func privot_tps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_tps
	return null


func privot_fps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_fps
	return null



func set_local_up( up: Vector3 ):
	local_up = up






func create_physical():
	var ph: Node =  _create_physical( PhysicalType )
	# Physics body calls "integrate_forces" method from this instance.
	# Becasue of that it is necesary to provide the reference to "self".
	# Check for "null" beacuse when there is no parent ref. frame, 
	# physics body is not created.
	if ph != null:
		ph.body = self




# This one should be called by CharacterPhysicsBody
func integrate_forces( body: RigidBody, state ):
	# If it is free floating, don't apply any forces. 
	# Just let it drift on its own.
	if not free_floating:
		_position_control( state )
	
	_ang_vel_control( state, preserve_vertical )
	_preserve_vertical( state, preserve_vertical )


func apply_force( body: RigidBody, f: Vector3 ):
	var t: Transform = self.transform
	var q: Quat = t.basis
	var fw: Vector3 = q.xform( f )
	body.add_central_force( fw )


func get_speed_normalized( body: RigidBody ):
	var t: Transform = body.transform
	var q: Quat = t.basis
	q = q.inverse()
	var v: Vector3 = body.linear_velocity
	v = q.xform( v )
	var current_speed: float = v.length()
	var s: float = current_speed / speed
	var current_velocity: Vector3 = v / speed
	return [s, current_velocity]













func _position_control( state ):
	var dt: float = state.step
	var v: Vector3 = Vector3.ZERO
	
	var w: bool = _input.has( "ui_w" )
	var s: bool = _input.has( "ui_s" )
	var a: bool = _input.has( "ui_a" )
	var d: bool = _input.has( "ui_d" )
	if w:
		v += Vector3.FORWARD
	if s:
		v += Vector3.BACK
	if a:
		v += Vector3.LEFT
	if d:
		v += Vector3.RIGHT
	if v.length_squared() > 0.0:
		v = v.normalized()
		v *= speed
	
	var t: Transform = self.transform
	
	if not translation_abolute:
		v = t.basis.xform( v )
	
	var model_v: Vector3 = self.linear_velocity
	var dv: Vector3 = v - model_v
	var f: Vector3 = gain * dv
	
	if translation_do_ignore_direction:
		var dir: Vector3 = translation_ignore_direction.normalized()
		if not translation_abolute:
			dir = t.basis.xform( dir )
		var ign_force = dir * dir.dot( f )
		f -= ign_force
	
	var f_abs = f.length()
	if f_abs > max_force:
		f = f * (max_force / f_abs)
	
	state.add_central_force( f )


func _ang_vel_control( state, preserving_vertical: bool = true ):
	var dt: float = state.step
	
	var w: Vector3 = Vector3.ZERO
	
	var i: bool = _input.has( "ui_i" )
	var k: bool = _input.has( "ui_k" )
	var j: bool = _input.has( "ui_j" )
	var l: bool = _input.has( "ui_l" )
	var u: bool = _input.has( "ui_u" )
	var o: bool = _input.has( "ui_o" )

	if i:
		w += Vector3.LEFT
	if k:
		w += Vector3.RIGHT
	if j:
		w += Vector3.BACK
	if l:
		w += Vector3.FORWARD
	if u:
		w += Vector3.UP
	if o:
		w += Vector3.DOWN
	
	if preserving_vertical:
		# Only take care of "w" projection along vertical.
		var up: Vector3 = Vector3.UP
		w = up * ( up.dot( w ) )
	
	if w.length_squared() > 0.0:
		w = w.normalized()
		w *= ang_vel #* dt * 0.5
	
	var wanted_w = w
	if not rotation_abolute:
		var t: Transform = self.transform
		wanted_w = t.basis.xform( wanted_w )
	wanted_w *= max_ang_vel
	var current_w: Vector3 = state.angular_velocity
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * gain_angular * dw
	var L = torque.length()

	if L > max_torque:
		torque = torque * (max_torque / L)
	
	state.add_torque( torque )



func _preserve_vertical( state, preserving_vertical: bool = true ):
	if not preserving_vertical:
		return
	
	var dt: float = state.step
	var t: Transform = self.transform
	var up: Vector3 = Vector3.UP
	up = t.basis.xform( up )
	# Want to convert "up" to "local_up".
	var da: Vector3 = up.cross( local_up )
	var wanted_w: Vector3 = da.normalized() * ang_vel
	var current_w: Vector3 = state.angular_velocity
	# Also subtract angular velocity projection along vertical.
	wanted_w = wanted_w - up * ( up.dot( wanted_w ) )
	
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * (gain_angular * dw - current_w * gain_d_angular)
	var L = torque.length()

	if L > max_torque:
		torque = torque * (max_torque / L)
	
	state.add_torque( torque )


func rotation_control( state ):
	var dt: float = state.step
	
	var w: Vector3 = Vector3.ZERO

	var i: bool = _input.has( "ui_i" )
	var k: bool = _input.has( "ui_k" )
	var j: bool = _input.has( "ui_j" )
	var l: bool = _input.has( "ui_l" )
	var u: bool = _input.has( "ui_u" )
	var o: bool = _input.has( "ui_o" )

	if i:
		w += Vector3.LEFT
	if k:
		w += Vector3.RIGHT
	if j:
		w += Vector3.BACK
	if l:
		w += Vector3.FORWARD
	if u:
		w += Vector3.UP
	if o:
		w += Vector3.DOWN
	
	if w.length_squared() > 0.0:
		w = w.normalized()
		w *= ang_vel * dt * 0.5
		
		var dq = Quat( w.x, w.y, w.z, 1.0 )
		if rotation_abolute:
			target_q = dq * target_q
		else:
			target_q = target_q * dq
		target_q = target_q.normalized()
	
	# Adjustment q.
	var t: Transform = self.transform
	var q: Quat = t.basis
	var dq = target_q * q.inverse()
	if dq.w < 0.0:
		dq = -dq
	#var angle = atan2( sqrt(1.0-dq.w*dq.w), dq )
	var wanted_w = Vector3( dq.x, dq.y, dq.z )
	if wanted_w.length_squared() > 0.0:
		wanted_w = wanted_w.normalized()
		wanted_w *= max_ang_vel
	
	var current_w: Vector3 = state.angular_velocity
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * (gain_angular * dw - current_w * gain_d_angular)
	var L = torque.length()

	if L > max_torque:
		torque = torque * (max_torque / L)
	
	state.add_torque( torque )



