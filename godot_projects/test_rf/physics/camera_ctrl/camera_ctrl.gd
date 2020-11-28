extends Camera

const Mode = {
	FPS         = 0, 
	TPS_AZIMUTH = 1,
	TPS_FREE    = 2
}


export(int) var mode   = Mode.TPS_AZIMUTH setget set_mode

var _ctrl_enabled: bool = false
var _mouse_displacement: Vector2 = Vector2.ZERO
var _zoom_displacement: int = 0

export(float) var sensitivity = 0.01 setget _set_sensitivity
export(float) var sensitivity_dist = 0.02

export(float) var dist_min = 1.0   setget _set_dist_min
export(float) var dist_max = 100.0 setget _set_dist_max
export(float) var limit_yaw   = 0.5
export(float) var limit_pitch = 1.57
export(float) var return_rate = 0.1

# All states share the same state set as one 
# as each set is a subset of this one.
var _state = {
	yaw   = 0.0, 
	pitch = 0.0, 
	dist  = 5.0, 
	quat = Quat.IDENTITY
}


# Camera unit vectors defining up axis and two horizontal axes.
var basis_initialized: bool = false
var e_x: Vector3 = Vector3(1.0, 0.0, 0.0)
var e_y: Vector3 = Vector3(0.0, 1.0, 0.0)
var e_z: Vector3 = Vector3(0.0, 0.0, 1.0)


var _target: Spatial = null




func set_mode( m ):
	mode = m
	apply_target()


func set_mode_fps():
	set_mode( Mode.FPS )


func set_mode_tps():
	set_mode( Mode.TPS_AZIMUTH )




func apply_target():
	var p = PhysicsManager.player_control
	if p:
		if mode == Mode.FPS:
			_target = p.privot_fps()
		else:
			_target = p.privot_tps()
	else:
		_target = null





func _cycle_mode():
	if mode == Mode.FPS:
		mode = Mode.TPS_AZIMUTH
	elif mode == Mode.TPS_AZIMUTH:
		mode = Mode.TPS_FREE
	elif mode == Mode.TPS_FREE:
		mode = Mode.FPS
	apply_target()





func _set_dist_min( v ):
	dist_min = v

func _set_dist_max( v ):
	dist_max = v

func _set_sensitivity( sens ):
	sensitivity = sens



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _input( event ):
	var gained_control: bool = Input.is_action_just_pressed( "ui_rmb" )
	if gained_control:
		_gain_control( true )
	else:
		var release_control: bool = Input.is_action_just_released( "ui_rmb" )
		if release_control:
			_gain_control( false )
	
	if event is InputEventMouseMotion:
		_mouse_displacement += event.relative
	var zoom_in: bool = Input.is_action_just_pressed( "ui_zoom_in" )
	if zoom_in:
		_zoom_displacement *= 0.7
	var zoom_out: bool = Input.is_action_just_pressed( "ui_zoom_out" )
	if zoom_out:
		_zoom_displacement *= 1.3
	
	#var pressed_c: bool = Input.is_action_just_pressed( "ui_c" )
	#if pressed_c:
	#	_cycle_target()

	#var change_mode: bool = Input.is_action_just_pressed( "ui_v" )
	#if change_mode:
	#	_cycle_mode()



func _process(_delta):
	if mode == Mode.FPS:
		_process_fps(_delta)
	elif mode == Mode.TPS_AZIMUTH:
		_process_tps_azimuth(_delta)
	elif mode == Mode.TPS_FREE:
		_process_tps_free(_delta)




func _gain_control( capture: bool ):
	_ctrl_enabled = capture
	if capture:
		Input.set_mouse_mode( Input.MOUSE_MODE_CAPTURED )
	else:
		Input.set_mouse_mode( Input.MOUSE_MODE_VISIBLE )



func _process_fps(_delta):
	if not is_instance_valid( _target ):
		return
	
	if _ctrl_enabled:
		var fr: float = -_mouse_displacement.x * sensitivity
		if (fr > 0.0) and (_state.yaw > limit_yaw):
			fr = 0.0
		elif (fr < 0.0) and (_state.yaw < -limit_yaw):
			fr = 0.0
		var rr: float
		if fr == 0.0:
			rr = _state.yaw * return_rate * _delta
		else:
			rr = 0.0

		_state.yaw +=  fr - rr
		
		fr = -_mouse_displacement.y * sensitivity
		if (fr > 0.0) and (_state.pitch > limit_pitch):
			fr = 0.0
		elif (fr < 0.0) and (_state.pitch < -limit_pitch):
			fr = 0.0
		if fr == 0.0:
			rr = _state.pitch * return_rate * _delta
		else:
			rr = 0.0
		_state.pitch +=  fr - rr
	
	var q: Quat = Quat( e_y, _state.yaw ) * Quat( e_x, _state.pitch )
	var t: Transform = Transform.IDENTITY
	t.basis = q
	t.origin = _target.global_transform.origin
	transform = t
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO
	


func _process_tps_azimuth( _delta ):
	if not is_instance_valid( _target ):
		return
	
	#print( "mouse displacement: ", _mouse_displacement )
	
	#apply_target()
	
	# Update the distance.
	var d_dist: float = sensitivity_dist * float(_zoom_displacement) * _state.dist
	_zoom_displacement = 0
	_state.dist += d_dist
	if _state.dist > dist_max:
		_state.dist = dist_max
	elif _state.dist < dist_min:
		_state.dist = dist_min
	
	if _ctrl_enabled:
		var fr: float = -_mouse_displacement.x * sensitivity
		#if (fr > 0.0) and (_state.yaw > limit_yaw):
		#	fr = 0.0
		#elif (fr < 0.0) and (_state.yaw < -limit_yaw):
		#	fr = 0.0
		var rr: float = 0.0 * _state.yaw * return_rate * _delta

		_state.yaw +=  fr - rr
		
		fr = -_mouse_displacement.y * sensitivity
		if (fr > 0.0) and (_state.pitch > limit_pitch):
			fr = 0.0
		elif (fr < 0.0) and (_state.pitch < -limit_pitch):
			fr = 0.0
		rr = 0.0 * _state.pitch * return_rate * _delta
		_state.pitch +=  fr - rr

	var q: Quat = Quat( e_y, _state.yaw ) * Quat( e_x, _state.pitch )
	var v_dist: Vector3 = Vector3( 0.0, 0.0, _state.dist )
	v_dist = q.xform( v_dist )
	
	var t: Transform = Transform.IDENTITY
	t.basis = q
	var target_origin: Vector3 = _target.global_transform.origin
	t.origin += v_dist + target_origin
	transform = t
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO



func _process_tps_free( _delta ):
	_process_tps_azimuth( _delta )




func _init_basis( up: Vector3 ):
	var Utils = preload( "res://physics/utils/orthogonalize.gd" )
	var ret = Utils.pick_basis( up )
	e_x = ret[0]
	e_y = ret[1]
	e_z = ret[2]


func _adjust_basis( up: Vector3 ):
	var Utils = preload( "res://physics/utils/orthogonalize.gd" )
	var ret = Utils.adjust_basis( e_x, e_z, up )
	e_x = ret[0]
	e_y = ret[1]
	e_z = ret[2]


func process_basis( up: Vector3 ):
	if basis_initialized:
		_adjust_basis( up )
	else:
		_init_basis( up )
		basis_initialized = true


func jump_basis( t_before: Transform, t_after: Transform ):
	var q_before: Quat = t_before.basis
	var q_after: Quat  = t_after.basis
	var q: Quat = q_before.inverse() * q_after
	q = q.inverse()
	e_x = q.xform( e_x )
	e_y = q.xform( e_y )
	e_z = q.xform( e_z )





