
extends Camera
class_name PlayerCamera

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
export(float) var sensitivity_dist = 0.2

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


var local_ref_frame: Quat = Quat.IDENTITY


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
	_init_basis()


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
		_zoom_displacement -= 1
	var zoom_out: bool = Input.is_action_just_pressed( "ui_zoom_out" )
	if zoom_out:
		_zoom_displacement += 1
	
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
	
	_process_sky()




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
	
	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	var t: Transform = Transform.IDENTITY
	q = local_ref_frame * q
	t.basis = Basis( q )
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
	if _zoom_displacement != 0:
		var d_dist: float = exp( log(1.0 + sensitivity_dist) * float(_zoom_displacement) )
		_state.dist *= d_dist
		if _state.dist > dist_max:
			_state.dist = dist_max
		elif _state.dist < dist_min:
			_state.dist = dist_min
		_zoom_displacement = 0
	
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

#	_state.yaw = PI / 2.0
#	_state.pitch = 0.0

	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	q = local_ref_frame * q
	var v_dist: Vector3 = Vector3( 0.0, 0.0, _state.dist )
	v_dist = q.xform( v_dist )
	
	var t: Transform = Transform.IDENTITY
	t.basis = Basis( q )
	var target_origin: Vector3 = _target.global_transform.origin
	t.origin += v_dist + target_origin
	transform = t
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO



func _process_tps_free( _delta ):
	_process_tps_azimuth( _delta )




func _init_basis():
	local_ref_frame = Quat.IDENTITY




func process_basis( up: Vector3 ):
	var current_up: Vector3 = local_ref_frame.xform( Vector3.UP )
	var rot: Vector3 = current_up.cross( up )
	var a: float = rot.length()
	var q: Quat
	if a < 0.000001:
		q = Quat.IDENTITY
	else:
		var a_2: float = a * 0.5
		var co2: float = cos( a_2 )
		var si2: float = sin( a_2 )
		var k: float = si2 / a
		q.w = co2
		q.x = rot.x*k
		q.y = rot.y*k
		q.z = rot.z*k
	
	local_ref_frame = q * local_ref_frame
	local_ref_frame = local_ref_frame.normalized()


func apply_atmosphere( player_ref_frame: RefFrame, celestial_body: CelestialSurface ):
	var planet_radius: float = celestial_body.radius_km * 1000.0
	var se3: Se3Ref = celestial_body.relative_to( player_ref_frame )
	var planet_t: Transform = se3.transform
	var r: Vector3 = planet_t.origin
	
	var player_parent = player_ref_frame.get_parent()
	#print( "planet origin: ", r, ", player parent: ", player_parent.name )
	
	var atm: MeshInstance = get_node( "Atmosphere" ) as MeshInstance
	if atm == null:
		return
	
	var far: float = self.far * 0.5
	var t: Transform = atm.transform
	t.basis = Basis.IDENTITY
	t.basis = t.basis.scaled( Vector3( far, far, far ) )
	atm.transform = t

	var atmosphere_height: float        = celestial_body.atmosphere_height_km * 1000.0
	var opaque_height: float            = celestial_body.opaque_height_km * 1000.0
	var transparency_scale_outer: float = celestial_body.transparency_scale_outer_km * 1000.0
	var transparency_scale_inner: float = celestial_body.transparency_scale_inner_km * 1000.0
	var displacement: float             = celestial_body.displacement
	
	var m: ShaderMaterial = atm.material_override as ShaderMaterial
	m.set_shader_param( "sphere_position",          r )
	m.set_shader_param( "sphere_radius",            planet_radius )
	m.set_shader_param( "atmosphere_height",        atmosphere_height )
	m.set_shader_param( "opaque_height",            opaque_height )
	m.set_shader_param( "transparency_scale_outer", transparency_scale_outer )
	m.set_shader_param( "transparency_scale_inner", transparency_scale_inner )
	m.set_shader_param( "displacement",             displacement )



func apply_sun( player_ref_frame: RefFrame, sun: Sun ):
	var atm: MeshInstance = get_node( "Atmosphere" ) as MeshInstance
	if atm == null:
		return
	# Determine relative position.
	var se3: Se3Ref = sun.relative_to( player_ref_frame )
	var dist: float = se3.r.length()
	var light_dir: Vector3 = se3.r.normalized()
	# Determine sun angular radius.
	var sz: float = sun.radius_km / dist * 1000.0
	
	var glow_size: float = sz * sun.glow_size
	var ray_scale: float = sun.ray_scale
	var ray_size: float  = sun.ray_size
	var ray_bias: float  = sun.ray_bias
	
	var m: ShaderMaterial = atm.material_override as ShaderMaterial
	m = m.next_pass as ShaderMaterial
	m.set_shader_param( "light_dir", light_dir )
	m.set_shader_param( "glow_size", glow_size )
	m.set_shader_param( "ray_scale", ray_scale )
	m.set_shader_param( "ray_size", ray_size )
	m.set_shader_param( "ray_bias", ray_bias )



func _process_sky():
	var player_rf: RefFrameNode = PhysicsManager.get_player_ref_frame()
	if player_rf == null:
		return
	
	var se3: Se3Ref = player_rf.relative_to( null )
	var q: Quat = self.global_transform.basis.get_rotation_quat()
	q = (se3.q * q).inverse()
	
	#print( "global q: ", q )
	
	var bg: Spatial = get_node( "Background" ) as Spatial
	
	var far: float = self.far * 0.9
	var t: Transform = bg.transform
	t.basis = q
	t.basis = t.basis.scaled( Vector3( far, far, far ) )
	
	bg.transform = t


