extends Spatial


export(NodePath) var target_path setget _set_target_path
var target = null

signal euler( angles )

var _mouse = {
	hover_x = false, 
	hover_y = false, 
	hover_z = false
}

var _dragging = {
	enabled = false, 
	origin = Vector3.ZERO, 
	rotation_axis = Vector3.ZERO, 
	drag_axis = Vector3.ZERO, 
	start_q = Quat.IDENTITY, 
	mouse_start = Vector3.ZERO, 
	mouse_at = Vector3.ZERO, 
	drag_scale = 1.0, 
	euler = Vector3.ZERO
}


# Called when the node enters the scene tree for the first time.
func _ready():
	_set_target_path( target_path )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( _delta ):
	if not _dragging.enabled:
		if target != null:
			var t: Transform = target.t()
			self.transform = t
			




func _set_target_path( t ):
	target_path = t
	target = get_node( t )
	if target:
		_dragging.transform = target.transform
		self.transform = target.transform


func _on_AreaX_mouse_entered():
	_mouse.hover_x = true


func _on_AreaX_mouse_exited():
	_mouse.hover_x = false


func _on_AreaZ_mouse_entered():
	_mouse.hover_z = true


func _on_AreaZ_mouse_exited():
	_mouse.hover_z = false


func _on_AreaY_mouse_entered():
	_mouse.hover_y = true


func _on_AreaY_mouse_exited():
	_mouse.hover_y = false


func _on_AreaX_input_event(camera, event, click_position, click_normal, shape_idx):
	_process_input( event )


func _on_AreaZ_input_event(camera, event, click_position, click_normal, shape_idx):
	_process_input( event )


func _on_AreaY_input_event(camera, event, click_position, click_normal, shape_idx):
	_process_input( event )


# It is here because need to process it not only when it hovers an axis.
func _input(event):
	var mm: InputEventMouse = event as InputEventMouse
	if mm:
		if _dragging.enabled:
			_process_dragging()

	var me: InputEventMouseButton = event as InputEventMouseButton
	if not me:
		return
	if (me.button_index != BUTTON_LEFT):
		return
	
	if not me.pressed:
		_dragging.enabled = false
		_finit_dragging()


func _process_input( event ):
	var me: InputEventMouseButton = event as InputEventMouseButton
	if not me:
		return
	if (me.button_index != BUTTON_LEFT):
		return
	
	if me.pressed:
		_dragging.enabled = true
	
	if _dragging.enabled:
		if _mouse.hover_x:
			_init_dragging( Vector3.RIGHT )
		elif _mouse.hover_y:
			_init_dragging( Vector3.UP )
		else:
			_init_dragging( Vector3.BACK )



func _init_dragging( axis: Vector3 ):
	# Own origin and unit vector.
	var t: Transform = self.transform
	var own_r: Vector3 = t.origin
	var own_a: Vector3 = t.basis.xform( axis )
	
	var vp: Viewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera = RootScene.ref_frame_root.player_camera
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	# Convert to ref. frame. object resides in.
	var gt: Transform = self.global_transform
	cam_r = t.basis.xform( gt.basis.xform_inv( cam_r ) )
	cam_a = t.basis.xform( gt.basis.xform_inv( cam_a ) )
	
	var drag_a: Vector3 = cam_a.cross( own_a )
	drag_a = drag_a.normalized()
	
	var q: Quat = t.basis
	
	var dist: float = (own_r - cam_r).length()
	
	var viewport_width: float = vp.get_visible_rect().size.x
	_dragging.origin = own_r
	_dragging.drag_scale      = 3.1415926535 / dist
	_dragging.rotation_axis   = own_a
	_dragging.drag_axis       = drag_a
	_dragging.start_q         = q
	_dragging.mouse_start = _mouse_on_axis()
	#print( "set mouse_start: ", _dragging.mouse_start )
	var euler: Vector3 = t.basis.get_euler()
	_dragging.euler = euler



func _process_dragging():
	_dragging.mouse_at = _mouse_on_axis()
	
	var dr: Vector3 = _dragging.mouse_at - _dragging.mouse_start
	var dx = dr.dot( _dragging.drag_axis )
	
	var angle: float = dx * _dragging.drag_scale
	
	var angle_2 = angle * 0.5
	var co2 = cos( angle_2 )
	var si2 = sin( angle_2 )
	var a: Vector3 = _dragging.rotation_axis
	var dq: Quat = Quat( a.x * si2, a.y * si2, a.z * si2, co2 )
	var q: Quat = dq * _dragging.start_q
	var b: Basis = q
	var euler: Vector3 = b.get_euler()
	euler.x = round( euler.x / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	euler.y = round( euler.y / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	euler.z = round( euler.z / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	q = Quat( euler )

	#print( "rot_axis: ", a, "drag_axis: ", _dragging.drag_axis, ", mouse_at: ", _dragging.mouse_at, ", dr: ", dr, ", dot: ", dx, ", angle: ", angle, ", euler: ", euler )
	
	
	if target != null:
		var body: PhysicsBodyBase = target as PhysicsBodyBase
		if body != null:
			body.set_q( q )
			body.update_physical_state_from_rf()
		else:
			var tt: Transform = target.transform
			tt.basis = q
			target.transform = tt
		var st: Transform = self.transform
		st.basis = q
		self.transform   = st

	
	if euler != _dragging.euler:
		_dragging.euler = euler
		emit_signal( "euler", euler )


func _mouse_on_axis():
	var r = _mouse_intersection()
	# Now project this point onto the line.
	var ro: Vector3 = _dragging.mouse_start
	var a: Vector3  = _dragging.drag_axis
	
	var dr: Vector3 = r - ro
	var proj: float = a.dot( dr )
	var dp: Vector3 = a * proj
	var p: Vector3  = dp + ro
	
	#print( "mouse_start: ", ro, ", drag_axis: ", a, ", mouse_at: ", r, ", on_axis: ", p )
	
	return p


func _mouse_intersection():
	var vp: Viewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera = RootScene.ref_frame_root.player_camera
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	# Convert to local ref. frame.
	#var t: Transform = self.global_transform
	#cam_r = t.basis.xform_inv( cam_r )
	#cam_a = t.basis.xform_inv( cam_a )
	
	# Axes origin and unit vector
	var own_a: Vector3 = _dragging.drag_axis
	var own_r: Vector3 = _dragging.origin
	
	var m: Basis
	m.x.x = 1.0 - own_a.x * own_a.x
	m.x.y = -own_a.y * own_a.x
	m.x.z = -own_a.z * own_a.x
	m.y.x = -own_a.x * own_a.y
	m.y.y = 1.0 - own_a.y * own_a.y
	m.y.z = -own_a.z * own_a.y
	m.z.x = -own_a.x * own_a.z
	m.z.y = -own_a.y * own_a.z
	m.z.z = 1.0 - own_a.z * own_a.z
	
	var own_A: Basis = m

	m.x.x = 1.0 - cam_a.x * cam_a.x
	m.x.y = -cam_a.y * cam_a.x
	m.x.z = -cam_a.z * cam_a.x
	m.y.x = -cam_a.x * cam_a.y
	m.y.y = 1.0 - cam_a.y * cam_a.y
	m.y.z = -cam_a.z * cam_a.y
	m.z.x = -cam_a.x * cam_a.z
	m.z.y = -cam_a.y * cam_a.z
	m.z.z = 1.0 - cam_a.z * cam_a.z
	
	var cam_A: Basis = m
	
	var A: Basis
	A.x.x = own_A.x.x + cam_A.x.x
	A.y.x = own_A.y.x + cam_A.y.x
	A.z.x = own_A.z.x + cam_A.z.x
	A.x.y = own_A.x.y + cam_A.x.y
	A.y.y = own_A.y.y + cam_A.y.y
	A.z.y = own_A.z.y + cam_A.z.y
	A.x.z = own_A.x.z + cam_A.x.z
	A.y.z = own_A.y.z + cam_A.y.z
	A.z.z = own_A.z.z + cam_A.z.z
	
	var inv_A: Basis = A.inverse()
	
	var own_p: Vector3 = own_A.xform( own_r )
	var cam_p: Vector3 = cam_A.xform( cam_r )
	var p: Vector3 = own_p + cam_p
	
	var r: Vector3 = inv_A.xform( p )
	
	#print( "\nmouse uv: ", mouse_uv, ", ro: ", cam_r, ", a: ", cam_a, ", closest_p: ", r )
	
	return r


func _finit_dragging():
	pass



