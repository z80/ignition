extends Camera

const Mode = {
	TPS_AZIMUTH = 0,
	TPS_FREE = 1, 
	FPS = 2
}

const Target = {
	PLAYER    = 0, 
	SELECTION = 1
}

var _mode: int   = Mode.STATE_TPS_AZIMUTH
var _target: int = Target.PLAYER
var _mouse_displacement: Vector2 = Vector2.ZERO
var _zoom_displacement: int = 0

export(int) var state setget _set_state

export(float) var sensitivity setget _set_sensitivity
export(float) var sensitivity_dist = 0.2

export(float) var dist_min = 1.0   setget _set_dist_min
export(float) var dist_max = 100.0 setget _set_dist_max
export(float) var limit_yaw   = 1.0
export(float) var limit_pitch = 1.0
export(float) var return_rate = 0.1

# All states share the same state set as one 
# as each set is a subset of this one.
var _state = {
	yaw   = 0.0, 
	pitch = 0.0, 
	dist  = 5.0, 
	quat = Quat.IDENTITY
}




var _target_tps: Spatial = null
var _target_fps: Spatial = null




func _set_state( st ):
	state = st

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
	if event is InputEventMouseMotion:
		_mouse_displacement += event.relative
	var zoom_in: bool = Input.is_action_just_pressed( "ui_zoom_in" )
	if zoom_in:
		_zoom_displacement -= 1
	var zoom_out: bool = Input.is_action_just_pressed( "ui_zoom_out" )
	if zoom_out:
		_zoom_displacement += 1



func _process(_delta):
	if _mode == Mode.FPS:
		_process_fps(_delta)
	elif _mode == Mode.TPS_AZIMUTH:
		_process_tps_azimuth(_delta)
	elif _mode == Mode.TPS_FREE:
		_process_tps_free(_delta)







func _process_fps(_delta):
	if not is_instance_valid( _target_fps ):
		return
	
	var fr: float
	if (_state.yaw > limit_yaw) or (_state.yaw < -limit_yaw):
		fr = 0.0
	else:
		fr = _mouse_displacement.x * sensitivity
	var rr: float = _state.yaw * return_rate * _delta

	_state.yaw +=  fr - rr
	
	if (_state.pitch > limit_pitch) or (_state.pitch < -limit_pitch):
		fr = 0.0
	else:
		fr = _mouse_displacement.y * sensitivity
	rr = _state.pitch * return_rate * _delta
	_state.pitch +=  fr - rr
	
	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	var t: Transform = _target_fps.transform
	var base_q: Quat = t.basis
	q = base_q * q
	t.basis = q
	
	transform = t
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO
	


func _process_tps_azimuth( _delta ):
	if not is_instance_valid( _target_tps ):
		return
	
	# Update the distance.
	var d_dist: float = sensitivity_dist * float(_zoom_displacement) * _state.dist
	_zoom_displacement = 0
	_state.dist += d_dist
	if _state.dist > dist_max:
		_state.dist = dist_max
	elif _state.dist < dist_min:
		_state.dist = dist_min
	
	var fr: float
	if (_state.yaw > limit_yaw) or (_state.yaw < -limit_yaw):
		fr = 0.0
	else:
		fr = _mouse_displacement.x * sensitivity
	var rr: float = _state.yaw * return_rate * _delta

	_state.yaw +=  fr - rr
	
	if (_state.pitch > limit_pitch) or (_state.pitch < -limit_pitch):
		fr = 0.0
	else:
		fr = _mouse_displacement.y * sensitivity
	rr = _state.pitch * return_rate * _delta
	_state.pitch +=  fr - rr
	
	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	var v_dist: Vector3 = Vector3( 0.0, 0.0, _state.dist )
	v_dist = q.xform( v_dist )
	
	var t: Transform = _target_fps.transform
	var base_q: Quat = t.basis
	q = base_q * q
	t.basis = q
	t.origin += v_dist
	
	
	transform = t
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO



func _process_tps_free( _delta ):
	_process_tps_azimuth( _delta )






