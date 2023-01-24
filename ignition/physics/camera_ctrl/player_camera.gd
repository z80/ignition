
extends RefFrameNode
class_name PlayerCamera

var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )

export(float) var near setget _set_near, _get_near
export(float) var far  setget _set_far,  _get_far
export(Vector3) var translation setget _set_translation, _get_translation
export(Transform) var global_transform setget _set_global_transform, _get_global_transform

const Mode = {
	FPS         = 0, 
	TPS_AZIMUTH = 1,
	TPS_FREE    = 2
}

export(bool) var map_mode = false setget _set_map_mode, _get_map_mode
export(int) var mode   = Mode.TPS_AZIMUTH setget set_mode

var _ctrl_enabled: bool = false
var _mouse_displacement: Vector2 = Vector2.ZERO
var _zoom_displacement: int = 0


var _sun_light: DirectionalLight = null


export(float) var sensitivity = 0.01 setget _set_sensitivity
export(float) var sensitivity_dist = 0.2
export(float) var map_distance_multiplier = 100.0

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
	last_dist = -1.0, 
	last_map_dist = -1.0, 
	quat = Quat.IDENTITY
}


var _target: Spatial = null

var _camera: Camera = null


func _get_camera():
	if _camera == null:
		_camera = get_node( "Camera" )
	return _camera


func _set_near( v: float ):
	var c: Camera = _get_camera()
	c.near = v


func _get_near():
	var c: Camera = _get_camera()
	var ret: float = c.near
	return ret


func _set_far( v: float ):
	var c: Camera = _get_camera()
	c.far = v


func _get_far():
	var c: Camera = _get_camera()
	var ret: float = c.far
	return ret


func _set_translation( v: Vector3 ):
	var c: Camera = _get_camera()
	c.translation = v


func _get_translation():
	var c: Camera = _get_camera()
	var ret: Vector3 = c.translation
	return ret


func _set_global_transform( t: Transform ):
	var c: Camera = _get_camera()
	c.global_transform = t


func _get_global_transform():
	var c: Camera = _get_camera()
	var t: Transform = c.global_transform
	return t


func get_camera():
	var camera: Camera = _get_camera()
	return camera


func unproject_position( world_at: Vector3 ):
	var c: Camera = _get_camera()
	var at_2d: Vector2 = c.unproject_position( world_at )
	return at_2d


func _set_map_mode( en: bool ):
	if en == false:
		if not map_mode:
			return
		self.transform = Transform.IDENTITY
		_state.last_map_dist = _state.dist
		if _state.last_dist > 0.0:
			_state.dist = _state.last_dist
		elif map_mode == true:
			_state.dist = _state.dist / map_distance_multiplier
	else:
		if map_mode:
			return
		_state.last_dist = _state.dist
		if _state.last_map_dist > 0.0:
			_state.dist = _state.last_map_dist
		elif map_mode == false:
			_state.dist = _state.dist * map_distance_multiplier
		# In map mode displace ref frame and keep camera at the origin with identity transform
		# all the time.
		var c: Camera = _get_camera()
		var t: Transform = c.transform
		self.transform = t
		c.transform = Transform.IDENTITY
	map_mode = en


func _get_map_mode():
	return map_mode


func set_mode( m: int ):
	mode = m
	apply_target()


func set_mode_fps():
	set_mode( Mode.FPS )


func set_mode_tps():
	set_mode( Mode.TPS_AZIMUTH )




func apply_target():
	var p: RefFrameNode = RootScene.ref_frame_root.player_control
	self.change_parent( p )
	if is_instance_valid( p ):
		if mode == Mode.FPS:
			_target = p.pivot_fps()
		else:
			_target = p.pivot_tps()
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






func _exit_tree():
	if is_queued_for_deletion():
		on_delete()


func on_delete():
	print( "Deleting the camera" )


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
	
	#print( "mouse displacement: ", _mouse_displacement )




# It should be called manually
func update( _delta: float ):
	var input: Dictionary = UserInput.get_input()
	var mm: bool = input.has( "ui_map" )
	_set_map_mode( mm )
	
	if map_mode:
		_process_map_mode( _delta )
	else:
		# In normal mode RefFrame transform is identity. And 
		# camera moves.
#		var t: Transform = Transform.IDENTITY
#		self.transform = t
		
		if mode == Mode.FPS:
			_process_fps(_delta)
		elif mode == Mode.TPS_AZIMUTH:
			_process_tps_azimuth(_delta)
		elif mode == Mode.TPS_FREE:
			_process_tps_free(_delta)
	
	_process_sky()
	
	_draw_broad_tree_faces()




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
	self.set_q( q )
	self.set_r( _target.transform.origin )
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO
	


func _process_tps_azimuth( _delta ):
	#var player_ctrl: RefFrameNode = RootScene.ref_frame_root.player_control
	var player_ctrl: RefFrameNode = get_parent()
	if not is_instance_valid( player_ctrl ):
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
	
	
	# Player relative to the nearest celestial body.
	var celestial_body: RefFrameNode = ClosestCelestialBody.closest_celestial_body( player_ctrl )
	if not is_instance_valid( celestial_body ):
		return
	
	# Adjust current orientation.
#	player_ctrl.debug = true
#	celestial_body.debug = true
#	self.debug = true
#	var se3_rel__: Se3Ref = RootScene.ref_frame_root.player_camera.relative_to( celestial_body )
	
	var se3_rel: Se3Ref = player_ctrl.relative_to( celestial_body )
	var wanted_up: Vector3 = se3_rel.r.normalized()
	
	var co: float = wanted_up.y
	var si: float = Vector2( wanted_up.x, wanted_up.z ).length()
	var elevation: float = atan2( si, co )
	var q_el: Quat = Quat( Vector3.RIGHT, elevation )
	
	co = wanted_up.z
	si = wanted_up.x
	var azimuth: float = atan2( si, co )
	var q_az: Quat = Quat( Vector3.UP, azimuth )
	
	var q_total: Quat = q_az * q_el
	
	var q_player_relative_to_celestial_body: Quat = se3_rel.q
	var inv_q_player_relative_to_celestial_body: Quat = q_player_relative_to_celestial_body.inverse()
	var q_camera_base: Quat = inv_q_player_relative_to_celestial_body * q_total
	
	
	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	q = q_camera_base * q
	self.set_q( q )
	
	
	var v_dist: Vector3 = Vector3( 0.0, 0.0, _state.dist )
	v_dist = q.xform( v_dist )
	
	var target_origin: Vector3
	if is_instance_valid(_target):
		target_origin = _target.transform.origin
	else:
		target_origin = Vector3.ZERO
	
	self.set_r( v_dist + target_origin )
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO



func _process_tps_free( _delta: float ):
	_process_tps_azimuth( _delta )




func _process_map_mode( _delta: float ):
	var player_ctrl: RefFrameNode = get_parent()
	
	if not is_instance_valid( player_ctrl ):
		return
	
	# Update the distance.
	if _zoom_displacement != 0:
		var d_dist: float = exp( log(1.0 + sensitivity_dist) * float(_zoom_displacement) )
		_state.dist *= d_dist
		# Yes, in map mode the smallest distance is max distance for normal mode.
		if _state.dist < dist_max:
			_state.dist = dist_max
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
	
	# Also need to know global Player ref frame orientation to undo it.
	var se3: Se3Ref = player_ctrl.relative_to( null )
	var base_q: Quat = se3.q
	base_q = base_q.inverse()
	
	var q: Quat = Quat( Vector3.UP, _state.yaw ) * Quat( Vector3.RIGHT, _state.pitch )
	q = base_q * q
	var v_dist: Vector3 = Vector3( 0.0, 0.0, _state.dist )
	v_dist = q.xform( v_dist )
	
	# Assigning camera ref. frame a pose.
	self.set_q( q )
	self.set_r( v_dist )
	
	# Zero mouse displacement as this thing is continuously accumulated.
	_mouse_displacement = Vector2.ZERO









func apply_atmosphere( celestial_body: RefFrameNode ):
	var atm: MeshInstance = get_node( "Camera/Atmosphere" ) as MeshInstance
	if atm == null:
		return
	var visible: bool = (celestial_body != null)
	atm.visible = visible
	if not visible:
		return
	
	var se3: Se3Ref = celestial_body.relative_to( self )
	var planet_center: Vector3 = se3.r
	
	# Relative to the Sun.
	se3 = self.relative_to( null )
	se3 = se3.inverse()
	var light_dir: Vector3 = se3.r.normalized()
	
	var player_parent: Node = self.get_parent()
	#print( "planet origin: ", r, ", player parent: ", player_parent.name )
	
	
	var f: float = self.far
	var far: float = f * 0.5
	var t: Transform = atm.transform
	t.basis = Basis.IDENTITY
	t.basis = t.basis.scaled( Vector3( far, far, far ) )
	atm.transform = t

	var planet_radius: float = celestial_body.radius_km * 1000.0
	var inner_height: float = celestial_body.atmosphere_height_inner_km * 1000.0
	var outer_height: float = celestial_body.atmosphere_height_outer_km * 1000.0
	var inner_dist: float   = celestial_body.transparency_dist_inner_km * 1000.0
	var outer_dist: float   = celestial_body.transparency_dist_outer_km * 1000.0
	var color_day: Color    = celestial_body.atmosphere_color_day
	var color_night: Color  = celestial_body.atmosphere_color_night
	var displacement: float = celestial_body.displacement


#	planet_radius += 2.5 * 1000.0
#	inner_height += 1.0 * 1000.0
#	outer_height += 1.0 * 1000.0
#	planet_radius = 1.0
	
	var m = atm.get_surface_material( 0 )
	m.set_shader_param( "planet_center",               planet_center )
	m.set_shader_param( "planet_radius",               planet_radius )
	m.set_shader_param( "light_dir",                   light_dir )
	m.set_shader_param( "inner_height",                inner_height )
	m.set_shader_param( "outer_height",                outer_height )
	m.set_shader_param( "inner_transparency_distance", inner_dist )
	m.set_shader_param( "outer_transparency_distance", outer_dist )
	m.set_shader_param( "displacement",                displacement )
	m.set_shader_param( "color_day",                   color_day )
	m.set_shader_param( "color_night",                 color_night )



func place_light( sun: RefFrameNode ):
	var se3: Se3Ref = sun.relative_to( self )
	#var inv_se3: Se3Ref = se3.inverse()
	
	var current_dir: Vector3 = Vector3( 0.0, 0.0, 1.0 )
	var wanted_dir: Vector3  = se3.r.normalized()
	var rot: Vector3 = current_dir.cross( wanted_dir )
	var si: float = rot.length()
	var co: float = current_dir.dot( wanted_dir )
	var angle: float = atan2( si, co )
	var q: Quat
	if si < 0.001:
		q = Quat.IDENTITY
	else:
		var n: Vector3 = rot / si
		var a_2: float = angle * 0.5
		var si_2: float = sin( a_2 )
		var co_2: float = cos( a_2 )
		n *= si_2
		q = Quat( n.x, n.y, n.z, co_2 )
	
	# Place light to the right place
	var b: Basis = Basis( q )
	RootScene.set_sun_direction( b )


func apply_sun( player_ref_frame: RefFrameNode, sun: RefFrameNode ):
	var sky: MeshInstance = get_node( "Camera/BackgroundSky" ) as MeshInstance
	if sky == null:
		return
	# Determine relative position.
	var se3: Se3Ref = self.relative_to( sun )
	place_light( sun )
	
	se3 = sun.relative_to( self )
	var dist: float = se3.r.length()
	var light_dir: Vector3 = se3.r.normalized()

	# Determine sun angular radius.
	var sz: float = sun.radius_km / dist * 1000.0
	
	var glow_size: float = sz * sun.glow_size
	var ray_scale: float = sun.ray_scale
	var ray_size: float  = sun.ray_size
	var ray_bias: float  = sun.ray_bias
	
	var m: SpatialMaterial = sky.get_surface_material( 0 ) as SpatialMaterial
	var ms: ShaderMaterial = m.next_pass as ShaderMaterial
	ms.set_shader_param( "light_dir", light_dir )
	ms.set_shader_param( "light_size", sz )
	ms.set_shader_param( "glow_size", glow_size )
	ms.set_shader_param( "ray_scale", ray_scale )
	ms.set_shader_param( "ray_size", ray_size )
	ms.set_shader_param( "ray_bias", ray_bias )



func _process_sky():
	var se3: Se3Ref = self.relative_to( null )
	var q: Quat = se3.q.inverse()
	
	#print( "global q: ", q )
	
	var bg: Spatial = get_node( "Camera/BackgroundSky" ) as Spatial
	
	var far: float = self.far * 0.9
	var t: Transform = bg.transform
	t.basis = q
	t.basis = t.basis.scaled( Vector3( far, far, far ) )
	
	bg.transform = t



func _get_sun_light():
	if _sun_light == null:
		_sun_light = get_node( "SunLight" )
#		if _sun_light != null:
#			_sun_light.omni_attenuation = 0.0
#			_sun_light.omni_range       = 100.0
	
	return _sun_light



func _draw_broad_tree_faces():
	var body: PhysicsBodyBase = get_parent() as PhysicsBodyBase
	if body == null:
		return
	
	var rf: RefFramePhysics = body.get_parent() as RefFramePhysics
	if rf == null:
		return
	
	var broad_tree: BroadTreeGd = rf.get_broad_tree()
	
	var visualization: Node = get_node( "BroadTreeVisualization" )
	visualization.draw_faces( broad_tree, self, rf )




func root_most_body():
	return null


func distance( other: RefFrameNode ):
	var se3: Se3Ref = self.relative_to( other )
	var v: Vector3 = se3.r
	var d: float = v.length()
	return d


func serialize():
	var data: Dictionary = {}
	data.mode = int(mode)
	data.ctrl_enabled = _ctrl_enabled
	data.mouse_displacement_x = _mouse_displacement.x
	data.mouse_displacement_y = _mouse_displacement.y
	data.zoom_displacement   =  _zoom_displacement
	return data





func deserialize( data: Dictionary ):
	mode = data.mode
	_ctrl_enabled = data.ctrl_enabled
	_mouse_displacement.x = data.mouse_displacement_x
	_mouse_displacement.y = data.mouse_displacement_y
	_zoom_displacement    = data.zoom_displacement
	apply_target()
	return true



