
extends Part
class_name Character

@export var speed: float     = 3.0
@export var gain: float      = 50.5
@export var max_force: float = 5.0


@export var ang_vel: float        = 1.0
@export var max_ang_vel: float    = 2.0
@export var gain_angular: float   = 900.0
@export var gain_d_angular: float = 100.0
@export var max_torque: float     = 50.0

@export var translation_abolute: bool = false
@export var rotation_abolute: bool    = false

# Amount of force projected onto this direction is zeroed.
@export var translation_do_ignore_direction: bool = true
@export var translation_ignore_direction: Vector3 = Vector3.UP


# If it is flying in space.
@export var free_floating: bool = false
# If it should resist tipping over.
@export var preserve_vertical: bool = true

var target_q: Quaternion = Quaternion.IDENTITY
var local_up: Vector3    = Vector3.UP

var print_period: float  = 0.1
var print_elapsed: float = 0.0

var _input: Dictionary = {}


# Need this mode in order to get in/out a habitat module.
enum BoardingMode {
	OUTSIDE=0, 
	INSIDE=1
}

var _boarding_mode = BoardingMode.OUTSIDE
@export var boarding_mode: BoardingMode: get = get_boarding_mode, set = set_boarding_mode


func set_boarding_mode( new_mode ):
	if new_mode == BoardingMode.INSIDE:
		set_boarding_mode_inside()
	elif new_mode == BoardingMode.OUTSIDE:
		set_boarding_mode_outside()


func set_boarding_mode_inside():
	remove_physical()
	_visual.visible = false
	body_state = BodyState.PASSIVE
	_boarding_mode = BoardingMode.INSIDE


func set_boarding_mode_outside():
	_visual.visible = true
	# First set modes.
	body_state    = BodyState.ACTIVE
	_boarding_mode = BoardingMode.OUTSIDE
	# After that update/create physical body as the result depends on 
	# the states provided.
	update_physics_from_state()


func get_boarding_mode():
	return _boarding_mode


func init():
	super.init()
	
	var PartControlGroups = load( "res://physics/parts/part_control_groups.gd" )
	control_group = PartControlGroups.ControlGroup._1


# Called when the node enters the scene tree for the first time.
func _ready():
	super._ready()
	_apply_default_orientation()


func _apply_default_orientation():
	var ClosestCelestialBody = preload( 'res://physics/utils/closest_celestial_body.gd' )
	var celestial_body: RefFrameNode = ClosestCelestialBody.closest_celestial_body( self )
	if not is_instance_valid( celestial_body ):
		return
	
	var se3_rel: Se3Ref = self.relative_to( celestial_body )
	var wanted_up: Vector3 = se3_rel.r.normalized()
	
	var co: float = wanted_up.y
	var si: float = Vector2( wanted_up.x, wanted_up.z ).length()
	var elevation: float = atan2( si, co )
	var q_el: Quaternion = Quaternion( Vector3.RIGHT, elevation )
	
	co = wanted_up.z
	si = wanted_up.x
	var azimuth: float = atan2( si, co )
	var q_az: Quaternion = Quaternion( Vector3.UP, azimuth )
	
	var q_total: Quaternion = q_az * q_el
	
	var se3: Se3Ref = get_se3()
	se3.q = q_total
	set_se3( se3 )
	update_physics_from_state()


# Copying user input into a local variable. So it can be processed 
# when it wants to do so.
func process_user_input_group( input: Dictionary ):
	_input = input.duplicate( true )


func process_inner( delta ):
	super.process_inner( delta )
	# Update visual animation state.
	if (_physical != null) and (_visual != null):
		# This is for visualizing walk.
		var ret: Array = get_speed_normalized( _physical )
		var s: float   = ret[0]
		var v: Vector3 = ret[1]
		_visual.set_speed_normalized( s, v )



func pivot_tps( _ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_tps
	return null


func pivot_fps( _ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_fps
	return null



func set_local_up( up: Vector3 ):
	local_up = up






func create_physical():
	# Check if not inside a habitat. Inside of a habitat don't create physics body.
	if boarding_mode == BoardingMode.INSIDE:
		return
	
	var ph: Node =  _create_physical( VisualScene )
	# Physics body calls "integrate_forces" method from this instance.
	# Becasue of that it is necesary to provide the reference to "self".
	# Check for "null" beacuse when there is no parent ref. frame, 
	# physics body is not created.
	if ph != null:
		ph.body = self




# This one should be called by CharacterPhysicsBody
func integrate_forces( _body: RigidBody3D, state ):
	# If it is free floating, don't apply any forces. 
	# Just let it drift on its own.
	if not free_floating:
		_position_control( state )
	
	_ang_vel_control( state, preserve_vertical )
	_preserve_vertical( state, preserve_vertical )


func apply_force(f: Vector3, body: RigidBody3D):
	var t: Transform3D = self.transform
	var q: Quaternion = t.basis
	var fw: Vector3 = q * (f)
	body.apply_central_force( fw )


func get_speed_normalized( body: RigidBody3D ):
	var t: Transform3D = body.transform
	var q: Quaternion = t.basis
	q = q.inverse()
	var v: Vector3 = body.linear_velocity
	v = q * (v)
	var current_speed: float = v.length()
	var s: float = current_speed / speed
	var current_velocity: Vector3 = v / speed
	return [s, current_velocity]













func _position_control( state ):
	var _dt: float = state.step
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
	
	var t: Transform3D = self.transform
	
	if not translation_abolute:
		v = t.basis * (v)
	
	var model_v: Vector3 = self.linear_velocity
	var dv: Vector3 = v - model_v
	var f: Vector3 = gain * dv
	
	if translation_do_ignore_direction:
		var dir: Vector3 = translation_ignore_direction.normalized()
		if not translation_abolute:
			dir = t.basis * (dir)
		var ign_force = dir * dir.dot( f )
		f -= ign_force
	
	var f_abs = f.length()
	if f_abs > max_force:
		f = f * (max_force / f_abs)
	
	state.apply_central_force( f )


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
		var t: Transform3D = self.transform
		wanted_w = t.basis * (wanted_w)
	wanted_w *= max_ang_vel
	var current_w: Vector3 = state.angular_velocity
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * gain_angular * dw
	var L = torque.length()

	if L > max_torque:
		torque = torque * (max_torque / L)
	
	state.apply_torque( torque )



func _preserve_vertical( state, preserving_vertical: bool = true ):
	if not preserving_vertical:
		return
	
	var dt: float = state.step
	var t: Transform3D = self.transform
	var up: Vector3 = Vector3.UP
	up = t.basis * (up)
	# Want to convert "up" to "local_up".
	var da: Vector3 = up.cross( local_up )
	var wanted_w: Vector3 = da.normalized() * ang_vel
	var current_w: Vector3 = state.angular_velocity
	# Also subtract angular velocity projection along vertical.
	wanted_w = wanted_w - up * ( up.dot( wanted_w ) )
	
	var dw = wanted_w - current_w
	var torque: Vector3 = dt * (gain_angular * dw) # - current_w * gain_d_angular)
	var L = torque.length()

	if L > max_torque:
		torque = torque * (max_torque / L)
	
	state.apply_torque( torque )


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
		
		var dq = Quaternion( w.x, w.y, w.z, 1.0 )
		if rotation_abolute:
			target_q = dq * target_q
		else:
			target_q = target_q * dq
		target_q = target_q.normalized()
	
	# Adjustment q.
	var t: Transform3D = self.transform
	var q: Quaternion = t.basis
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
	
	state.apply_torque( torque )



func _serialize( data: Dictionary ):
	data["boarding_mode"] = boarding_mode
	return data



func _deserialize( data: Dictionary ):
	
	if data.has( "boarding_mode" ):
		boarding_mode = data["boarding_mode"]
		if boarding_mode == BoardingMode.INSIDE:
			set_boarding_mode_inside()
		else:
			set_boarding_mode_outside()
	return true



func is_character():
	return true



