
extends RigidBody

var body: Body = null


const SPEED = 3.0
const GAIN = 50.5
const MAX_FORCE = 5.0


const ANG_VEL = 1.0
const MAX_ANG_VEL = 2.0
const GAIN_ANGULAR = 100.0
const GAIN_D_ANGULAR = 100.0
const MAX_TORQUE = 5.0

export(bool) var translation_abolute = false
export(bool) var rotation_abolute = false

# Amount of force projected onto this direction is zeroed.
export(bool) var translation_do_ignore_direction = true
export(Vector3) var translation_ignore_direction = Vector3.UP

var target_q: Quat = Quat.IDENTITY

var print_period: float = 0.1
var print_elapsed: float = 0.0

var apply_user_input: bool = false
var free_floating: bool = false

func _init():
	pass



func _integrate_forces( state ):
	if apply_user_input:
		apply_user_input = false
	
	# If it is free floating, don't apply any forces. 
	# Just let it drift on its own.
	if free_floating:
		return
	
	position_control( state )
	ang_vel_control( state )




func set_collision_layer( v: int ):
	self.collision_layer = v
	self.collision_mask  = v


func get_collision_layer():
	return self.collision_layer


func apply_force( f: Vector3 ):
	var t: Transform = self.transform
	var q: Quat = t.basis
	var fw: Vector3 = q.xform( f )
	self.add_central_force( fw )


func get_speed_normalized():
	var v: Vector3 = self.linear_velocity
	var s: float = v.length() / SPEED
	return s













func position_control( state ):
	var dt: float = state.step
	var v: Vector3 = Vector3.ZERO
	if apply_user_input:
		var w: bool = Input.is_action_pressed( "ui_w" )
		var s: bool = Input.is_action_pressed( "ui_s" )
		var a: bool = Input.is_action_pressed( "ui_a" )
		var d: bool = Input.is_action_pressed( "ui_d" )
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
		v *= SPEED
	
	if not translation_abolute:
		var t: Transform = self.transform
		v = t.basis.xform( v )
	
	var model_v: Vector3 = self.linear_velocity
	var dv: Vector3 = v - model_v
	var f: Vector3 = GAIN * dv
	
	if translation_do_ignore_direction:
		var dir: Vector3 = translation_ignore_direction.normalized()
		var ign_force = dir * dir.dot( f )
		f -= ign_force
	
	var f_abs = f.length()
	if f_abs > MAX_FORCE:
		f = f * (MAX_FORCE / f_abs)
	
	state.add_central_force( f )
	#self.add_central_force( f )


func ang_vel_control( state ):
	var dt: float = state.step
	print_elapsed += dt
	var do_print: bool
	if print_elapsed >= print_period:
		print_elapsed -= print_period
		do_print = true
		print( "" )
	else:
		do_print = false
	
	
	var w: Vector3 = Vector3.ZERO
	
	if apply_user_input:
		var i: bool = Input.is_action_pressed( "ui_i" )
		var k: bool = Input.is_action_pressed( "ui_k" )
		var j: bool = Input.is_action_pressed( "ui_j" )
		var l: bool = Input.is_action_pressed( "ui_l" )
		var u: bool = Input.is_action_pressed( "ui_u" )
		var o: bool = Input.is_action_pressed( "ui_o" )

		if do_print:
			print( "inputs: ", i, " ", k, " ", j, " ", l, " ", u, " ", o )
		if i:
			w += Vector3.RIGHT
			if do_print:
				print( "add LEFT ", w )
		if k:
			w += Vector3.LEFT
			if do_print:
				print( "add RIGHT ", w )
		if j:
			w += Vector3.UP
			if do_print:
				print( "add UP ", w )
		if l:
			w += Vector3.DOWN
			if do_print:
				print( "add DOWN ", w )
		if u:
			w += Vector3.BACK
			if do_print:
				print( "add BACK ", w )
		if o:
			w += Vector3.FORWARD
			if do_print:
				print( "add FORWARD ", w )
	if w.length_squared() > 0.0:
		w = w.normalized()
		if do_print:
			print( "control w: ", w )
		w *= ANG_VEL #* dt * 0.5
	
	#var dq = Quat( w.x, w.y, w.z, 1.0 )
	#target_q = target_q * dq
	#target_q = target_q.normalized()
	#if do_print:
	#	print( "target q: ", target_q )
	
	# Adjustment q.
	var t: Transform = self.transform
	var q: Quat = t.basis
	#if do_print:
	#	print( "current q: ", q )
	#dq = q.inverse() * target_q
	#if do_print:
	#	print( "relative q: ", dq )
	#var wanted_w = Vector3( dq.x, dq.y, dq.z )
	#if wanted_w.length_squared() > 0.0:
	#	wanted_w = wanted_w.normalized()
	
	var wanted_w = w
	if do_print:
		print( "wanted w relative: ", wanted_w )
	if not rotation_abolute:
		wanted_w = q.xform( wanted_w )
	wanted_w *= MAX_ANG_VEL
	if do_print:
		print( "wanted w absolute: ", wanted_w )
	var current_w: Vector3 = state.angular_velocity
	if do_print:
		print( "current w absolute: ", current_w )
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * GAIN_ANGULAR * dw
	var L = torque.length()

	if L > MAX_TORQUE:
		torque = torque * (MAX_TORQUE / L)
	
	if do_print:
		print( "torque: ", torque )
	state.add_torque( torque )





func rotation_control( state ):
	var dt: float = state.step
	print_elapsed += dt
	var do_print: bool
	if print_elapsed >= print_period:
		print_elapsed -= print_period
		do_print = true
		print( "" )
	else:
		do_print = false
	
	
	var w: Vector3 = Vector3.ZERO

	if apply_user_input:
		var i: bool = Input.is_action_pressed( "ui_i" )
		var k: bool = Input.is_action_pressed( "ui_k" )
		var j: bool = Input.is_action_pressed( "ui_j" )
		var l: bool = Input.is_action_pressed( "ui_l" )
		var u: bool = Input.is_action_pressed( "ui_u" )
		var o: bool = Input.is_action_pressed( "ui_o" )

		if do_print:
			print( "inputs: ", i, " ", k, " ", j, " ", l, " ", u, " ", o )
		if i:
			w += Vector3.RIGHT
			if do_print:
				print( "add LEFT ", w )
		if k:
			w += Vector3.LEFT
			if do_print:
				print( "add RIGHT ", w )
		if j:
			w += Vector3.UP
			if do_print:
				print( "add UP ", w )
		if l:
			w += Vector3.DOWN
			if do_print:
				print( "add DOWN ", w )
		if u:
			w += Vector3.BACK
			if do_print:
				print( "add BACK ", w )
		if o:
			w += Vector3.FORWARD
			if do_print:
				print( "add FORWARD ", w )
	
	if w.length_squared() > 0.0:
		w = w.normalized()
		if do_print:
			print( "control w: ", w )
		w *= ANG_VEL * dt * 0.5
		
		var dq = Quat( w.x, w.y, w.z, 1.0 )
		if rotation_abolute:
			target_q = dq * target_q
		else:
			target_q = target_q * dq
		target_q = target_q.normalized()
	if do_print:
		print( "target q: ", target_q )
	
	# Adjustment q.
	var t: Transform = self.transform
	var q: Quat = t.basis
	if do_print:
		print( "current q: ", q )
	var dq = target_q * q.inverse()
	if dq.w < 0.0:
		dq = -dq
	if do_print:
		print( "relative q: ", dq )
	#var angle = atan2( sqrt(1.0-dq.w*dq.w), dq )
	var wanted_w = Vector3( dq.x, dq.y, dq.z )
	if wanted_w.length_squared() > 0.0:
		wanted_w = wanted_w.normalized()
		wanted_w *= MAX_ANG_VEL
	
	if do_print:
		print( "wanted w absolute: ", wanted_w )
	var current_w: Vector3 = state.angular_velocity
	if do_print:
		print( "current w absolute: ", current_w )
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * (GAIN_ANGULAR * dw - current_w * GAIN_D_ANGULAR)
	var L = torque.length()

	if L > MAX_TORQUE:
		torque = torque * (MAX_TORQUE / L)
	
	if do_print:
		print( "torque: ", torque )
	state.add_torque( torque )




