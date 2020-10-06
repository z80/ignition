extends CanvasItem
class_name ManipulatorGrab


export(NodePath) var target_path setget set_path
var target: Node = null

# size dimensions.
export(float) var axis_length = 1.0
export(float) var origin_dist = 10.0
export(float) var sensitivity_radius = 12.0

const COLOR_X: Color = Color( 0.7, 0.0, 0.0, 0.7 )
const COLOR_Y: Color = Color( 0.0, 0.7, 0.0, 0.7 )
const COLOR_Z: Color = Color( 0.0, 0.0, 0.7, 0.7 )

const COLOR_X_HOVER: Color = Color( 1.0, 0.0, 0.0, 1.0 )
const COLOR_Y_HOVER: Color = Color( 0.0, 1.0, 0.0, 1.0 )
const COLOR_Z_HOVER: Color = Color( 0.0, 0.0, 1.0, 1.0 )

const LINE_WIDTH: float = 2.3

var _dragging = {
	enabled = false, 
	axis = Vector3.ZERO, 
	origin = Vector3.ZERO, 
	mouse_start = Vector3.ZERO, 
	mouse_at = Vector3.ZERO, 
	position = Vector3.ZERO
}

var _draw_params = {
	origin = Vector2.ZERO,
	axis_x = Vector2.ZERO, 
	axis_y = Vector2.ZERO, 
	axis_z = Vector2.ZERO, 
	hover_x = false, 
	hover_y = false, 
	hover_z = false
}



func set_path( path ):
	target_path = path
	target = get_node( target_path )


# Called when the node enters the scene tree for the first time.
func _ready():
	set_path( target_path )



func _draw():
	var dp = _draw_params
	var c: Color
	var to: Vector2
	
	# Draw X
	if _draw_params.hover_x:
		c = COLOR_X_HOVER
	else:
		c = COLOR_X
	to = dp.origin+dp.axis_x
	draw_line( dp.origin, to, c, LINE_WIDTH, false )
	draw_circle( to, sensitivity_radius, c )
	
	# Draw Y
	if _draw_params.hover_y:
		c = COLOR_Y_HOVER
	else:
		c = COLOR_Y
	to = dp.origin+dp.axis_y
	draw_line( dp.origin, to, c, LINE_WIDTH, false )
	draw_circle( to, sensitivity_radius, c )
	
	# Draw Z
	if _draw_params.hover_z:
		c = COLOR_Z_HOVER
	else:
		c = COLOR_Z
	to = dp.origin+dp.axis_z
	draw_line( dp.origin, to, c, LINE_WIDTH, false )
	draw_circle( to, sensitivity_radius, c )



# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var draw = _compute_draw_params()
	if draw == null:
		return
	var do_redraw: bool = _need_rewraw( draw )
	
	if do_redraw:
		_draw_params = draw
		update()



func _input( event ):
	var me: InputEventMouseButton = event as InputEventMouseButton
	if me != null:
		if (me.button_index != BUTTON_LEFT):
			return
		
		if me.pressed:
			if not _dragging.enabled:
				if _draw_params.hover_x:
					_init_dragging( Vector3.RIGHT )
				elif _draw_params.hover_y:
					_init_dragging( Vector3.UP )
				else:
					_init_dragging( Vector3.BACK )
		else:
			if _dragging.enabled:
				_finit_dragging()
		
		return
	
	var mm: InputEventMouseMotion = event as InputEventMouseMotion
	if mm != null:
		if _dragging.enabled:
			_process_dragging()





func _need_rewraw( draw ):
	for k in _draw_params:
		var v0 = _draw_params[k]
		var v1 = draw[k]
		if v0 != v1:
			return true
	
	return false


func _compute_draw_params():
	if target == null:
		return null
	var vp: Viewport = get_viewport()
	if vp == null:
		return null
	var cam = vp.get_camera()
	if cam == null:
		return null
	
	var t_target: Transform
	var b: Body = target as Body
	if b != null:
		t_target = b.t()
	else:
		t_target = target.global_transform
	var t_camera: Transform = cam.global_transform
	
	var t: Transform = t_camera.inverse() #* t_target
	
	var origin: Vector2 = cam.unproject_position( t_target.origin )
	
	var oo3: Vector3 = Vector3( 0.0, 0.0, -origin_dist )
	oo3 = t_camera.xform( oo3 )
	var oo = cam.unproject_position( oo3 )
	
	var ox3: Vector3 = Vector3( axis_length, 0.0, 0.0 )
	ox3 += oo3
	var ox: Vector2 = cam.unproject_position( ox3 )
	ox -= oo
	
	var oy3: Vector3 = Vector3( 0.0, axis_length, 0.0 )
	oy3 += oo3
	var oy: Vector2 = cam.unproject_position( oy3 )
	oy -= oo
	
	var oz3: Vector3 = Vector3( 0.0, 0.0, axis_length )
	oz3 += oo3
	var oz: Vector2 = cam.unproject_position( oz3 )
	oz -= oo
	
	# Mouse cursor position.
	var at: Vector2 = vp.get_mouse_position()
	var dist_x: float = (origin + ox - at).length() - sensitivity_radius
	var dist_y: float = (origin + oy - at).length() - sensitivity_radius
	var dist_z: float = (origin + oz - at).length() - sensitivity_radius

	var draw = {
		origin = origin, 
		axis_x = ox, 
		axis_y = oy, 
		axis_z = oz, 
		hover_x = (dist_x <= 0.0), 
		hover_y = (dist_y <= 0.0), 
		hover_z = (dist_z <= 0.0)
	}
	
	return draw




func _init_dragging( axis: Vector3 ):
	if _dragging.enabled:
		return
	
	_dragging.enabled = true
	
	# Own origin and unit vector.
	var t: Transform = self.transform
	if target == null:
		return
	var b: Body = target as Body
	if b != null:
		t = b.t()
	else:
		t = target.transform

	var own_r: Vector3 = t.origin
	var own_a: Vector3 = t.basis.xform( axis )
	
	_dragging.axis   = own_a
	_dragging.origin = own_r
	_dragging.mouse_start = _mouse_on_axis()


func _process_dragging():
	_dragging.mouse_at = _mouse_on_axis()
	
	var r: Vector3 = _dragging.mouse_at - _dragging.mouse_start + _dragging.origin
	
	var sz: float = Constants.CONSTRUCTION_GRID_SZ
	var x: float = r.x / sz
	var y: float = r.y / sz
	var z: float = r.z / sz
	x = round( x ) * sz
	y = round( y ) * sz
	z = round( z ) * sz
	
	r.x = x
	r.y = y
	r.z = z
	
	# Here probably need to convert back to local 
	# first as all vectors are in global ref frame
	var to: Vector3 = Vector3( x, y, z )
	if target != null:
		var b: Body = target as Body
		if b != null:
			b.set_r( to )
			b.update_physical_state_from_rf()
		else:
			target.translation = to
	
	if r != _dragging.position:
		_dragging.position = r
		emit_signal( "position", r )


func _mouse_on_axis():
	var r = _mouse_intersection()
	# Now project this point onto the line.
	var ro: Vector3 = _dragging.mouse_start
	var a: Vector3  = _dragging.axis
	
	var dr: Vector3 = r - ro
	var proj: float = a.dot( dr )
	var dp: Vector3 = a * proj
	var p: Vector3  = dp + ro
	
	return p


func _mouse_intersection():
	var vp: Viewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera = vp.get_camera()
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	# Convert to local ref. frame.
	var t: Transform = self.global_transform
	cam_r = t.basis.xform_inv( cam_r )
	cam_a = t.basis.xform_inv( cam_a )
	
	# Axes origin and unit vector
	var own_a: Vector3 = _dragging.axis
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
	
	#print( "mouse uv: ", mouse_uv, ", ro: ", cam_r, ", a: ", cam_a )
	
	return r


func _finit_dragging():
	_dragging.enabled = false


