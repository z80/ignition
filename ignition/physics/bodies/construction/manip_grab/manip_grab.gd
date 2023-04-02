extends CanvasItem
class_name ManipulatorGrabNew

signal drag_started
signal drag_finished


@export var target_path: NodePath: set = set_path
var target: Node = null

# Use parent's space or local object's space.
@export var use_local_space: bool = false

var _buttons: Control = null

# size dimensions.
@export var axis_length: float = 3.0
@export var origin_dist: float = 10.0
@export var sensitivity_radius: float = 12.0

const COLOR_X: Color = Color( 0.7, 0.0, 0.0, 0.7 )
const COLOR_Y: Color = Color( 0.0, 0.7, 0.0, 0.7 )
const COLOR_Z: Color = Color( 0.0, 0.0, 0.7, 0.7 )

const COLOR_X_HOVER: Color = Color( 1.0, 0.0, 0.0, 1.0 )
const COLOR_Y_HOVER: Color = Color( 0.0, 1.0, 0.0, 1.0 )
const COLOR_Z_HOVER: Color = Color( 0.0, 0.0, 1.0, 1.0 )

const LINE_WIDTH: float = 2.3

const CIRCLE_SEGMENTS_QTY: int = 32

var _dragging = {
	enabled = false, 
	axis = Vector3.ZERO, 
	origin = Vector3.ZERO, 
	axis_x = Vector2.ZERO, 
	axis_y = Vector2.ZERO, 
	axis_z = Vector2.ZERO, 
	mouse_start = Vector3.ZERO, 
	mouse_at = Vector3.ZERO, 
	position = Vector3.ZERO, 
	
	rot_enabled = false, 
	rotation_scale = 1.0, 
	start_q = Quaternion.IDENTITY, 
	euler   = Vector3.ZERO
}

var _draw_params = {
	origin = Vector2.ZERO,
	axis_x = Vector2.ZERO, 
	axis_y = Vector2.ZERO, 
	axis_z = Vector2.ZERO, 
	hover_x = false, 
	hover_y = false, 
	hover_z = false, 
	hover_rot_x = false, 
	hover_rot_y = false, 
	hover_rot_z = false
}



func set_path( path ):
	target_path = path
	target = get_node( target_path )


# Called when the node enters the scene tree for the first time.
func _ready():
	set_path( target_path )
	var root: Control = RootScene.get_root_for_gui_popups()
	var Buttons: PackedScene = load( "res://physics/bodies/construction/manip_grab/buttons.tscn" )
	_buttons = Buttons.instantiate()
	root.add_child( _buttons )
	_buttons.connect("world", Callable(self, "_on_world"))
	_buttons.connect("local", Callable(self, "_on_local"))

#	root.visible = true
#	_buttons.visible = true



func _exit_tree():
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()


func on_delete():
	if is_instance_valid( _buttons ):
		_buttons.queue_free()
		_buttons = null



func _draw():
	_draw_drag_axes()
	_draw_rot_circles()


func _draw_drag_axes():
	var dp = _draw_params
	var c: Color
	var to: Vector2
	
	# Draw X
	if _draw_params.hover_x:
		c = COLOR_X_HOVER
	else:
		c = COLOR_X
	to = dp.origin+dp.axis_x
	draw_line(dp.origin, to, c, LINE_WIDTH)
	draw_circle( to, sensitivity_radius, c )
	
	# Draw Y
	if _draw_params.hover_y:
		c = COLOR_Y_HOVER
	else:
		c = COLOR_Y
	to = dp.origin+dp.axis_y
	draw_line(dp.origin, to, c, LINE_WIDTH)
	draw_circle( to, sensitivity_radius, c )
	
	# Draw Z
	if _draw_params.hover_z:
		c = COLOR_Z_HOVER
	else:
		c = COLOR_Z
	to = dp.origin+dp.axis_z
	draw_line(dp.origin, to, c, LINE_WIDTH)
	draw_circle( to, sensitivity_radius, c )


func _draw_rot_circles():
	var dp = _draw_params
	var c: Color
	var to: Vector2
	
	# Draw X
	if _draw_params.hover_rot_x:
		c = COLOR_X_HOVER
	else:
		c = COLOR_X
	var center: Vector2 = dp.origin #- dp.axis_x
	var e1: Vector2 = dp.axis_y * 0.7
	var e2: Vector2 = dp.axis_z * 0.7
	_draw_rot_circle( center, e1, e2, c )
	
	# Draw Y
	if _draw_params.hover_rot_y:
		c = COLOR_Y_HOVER
	else:
		c = COLOR_Y
	center = dp.origin #- dp.axis_y
	e1 = dp.axis_x * 0.7
	e2 = dp.axis_z * 0.7
	_draw_rot_circle( center, e1, e2, c )
	
	# Draw Z
	if _draw_params.hover_rot_z:
		c = COLOR_Z_HOVER
	else:
		c = COLOR_Z
	center = dp.origin #- dp.axis_z
	e1 = dp.axis_x * 0.7
	e2 = dp.axis_y * 0.7
	_draw_rot_circle( center, e1, e2, c )




func _draw_rot_circle( center: Vector2, e1: Vector2, e2: Vector2, color: Color ):
	var qty_1 = float(CIRCLE_SEGMENTS_QTY-1)
	for i in range(CIRCLE_SEGMENTS_QTY):
		var a1 = PI * 2.0 * float(i)   / qty_1
		var a2 = PI * 2.0 * float(i+1) / qty_1
		var co1 = cos(a1)
		var si1 = sin(a1)
		var co2 = cos(a2)
		var si2 = sin(a2)
		var v1: Vector2 = center + co1*e1 + si1*e2
		var v2: Vector2 = center + co2*e1 + si2*e2
		draw_line(v1, v2, color, LINE_WIDTH)
	
	var c: Vector2 = center + 0.707 * ( e1 + e2 )
	draw_circle( c, sensitivity_radius, color )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var ret: Array =  _compute_draw_params()
	var draw: Dictionary = ret[0]
	if draw == null:
		return
	var axes: Dictionary = ret[1]
	_dragging.axis_x = axes.axis_x
	_dragging.axis_y = axes.axis_y
	_dragging.axis_z = axes.axis_z
	var do_redraw: bool = _need_redraw( draw )
	
	if do_redraw:
		_draw_params = draw
		update()



func _input( event ):
	var me: InputEventMouseButton = event as InputEventMouseButton
	if me != null:
		if (me.button_index != MOUSE_BUTTON_LEFT):
			return
		
		if me.pressed:
			if not _dragging.enabled:
				if _draw_params.hover_x:
					_init_dragging( _dragging.axis_x )
				elif _draw_params.hover_y:
					_init_dragging( _dragging.axis_y )
				elif _draw_params.hover_z:
					_init_dragging( _dragging.axis_z )
				if _draw_params.hover_rot_x:
					_init_rotating( _dragging.axis_x )
				elif _draw_params.hover_rot_y:
					_init_rotating( _dragging.axis_y )
				elif _draw_params.hover_rot_z:
					_init_rotating( _dragging.axis_z )
		else:
			if _dragging.enabled:
				_finit_dragging()
			elif _dragging.rot_enabled:
				_finit_rotating()
		
		return
	
	var mm: InputEventMouseMotion = event as InputEventMouseMotion
	if mm != null:
		if _dragging.enabled:
			_process_dragging()
		elif _dragging.rot_enabled:
			_process_rotating()





func _need_redraw( draw ):
	for k in _draw_params:
		var v0 = _draw_params[k]
		var v1 = draw[k]
		if v0 != v1:
			return true
	
	return false


func _compute_draw_params():
	if target == null:
		return null
	var vp: SubViewport = get_viewport()
	if vp == null:
		return null
	var cam = vp.get_camera_3d()
	if cam == null:
		return null
	
	# Axes in target parent ref frame.
	var target_parent_rf: RefFrameNode = target.get_parent()
	var target_rf_tr: Transform3D        = target.transform
	var cam_rf: RefFrameNode           = RootScene.ref_frame_root.player_camera
	var cam_rf_se3: Se3Ref             = cam_rf.relative_to( target_parent_rf )
	var cam_rf_tr: Transform3D           = cam_rf_se3.transform
	var target_parent_to_camera_rf_tr: Transform3D = cam_rf_tr.inverse()
	var target_rf_to_camera_rf_tr: Transform3D = target_parent_to_camera_rf_tr * target_rf_tr
	
	var target_se3: Se3Ref = target.get_se3()
	var target_q: Quaternion     = target_se3.q

	var t_camera: Transform3D = cam.global_transform
	
	var origin_3d_in_parent: Vector3 = target_rf_tr.origin
	var origin_3d: Vector3           = target_parent_to_camera_rf_tr * (origin_3d_in_parent) #origin.normalized() * origin_dist
	var origin_2d: Vector2           = cam.unproject_position( origin_3d )
	
	var ox3_w: Vector3 = Vector3( axis_length, 0.0, 0.0 )
	if use_local_space:
		ox3_w = target_q * (ox3_w)
	var axis_x_w: Vector3 = ox3_w.normalized()
	ox3_w = target_parent_to_camera_rf_tr * (ox3_w + origin_3d_in_parent)
	var ox: Vector2 = cam.unproject_position( ox3_w )
	ox -= origin_2d
	
	var oy3_w: Vector3 = Vector3( 0.0, axis_length, 0.0 )
	if use_local_space:
		oy3_w = target_q * (oy3_w)
	var axis_y_w: Vector3 = oy3_w.normalized()
	oy3_w = target_parent_to_camera_rf_tr * (oy3_w + origin_3d_in_parent)
	var oy: Vector2 = cam.unproject_position( oy3_w )
	oy -= origin_2d
	
	var oz3_w: Vector3 = Vector3( 0.0, 0.0, axis_length )
	if use_local_space:
		oz3_w = target_q * (oz3_w)
	var axis_z_w: Vector3 = oz3_w.normalized()
	oz3_w = target_parent_to_camera_rf_tr * (oz3_w + origin_3d_in_parent)
	var oz: Vector2 = cam.unproject_position( oz3_w )
	oz -= origin_2d
	
	# Mouse cursor position.
	var at: Vector2 = vp.get_mouse_position()
	var dist_x: float = (origin_2d + ox - at).length() - sensitivity_radius
	var dist_y: float = (origin_2d + oy - at).length() - sensitivity_radius
	var dist_z: float = (origin_2d + oz - at).length() - sensitivity_radius

	var dist_rot_x: float = (origin_2d + 0.7*0.707*(oy+oz) - at).length() - sensitivity_radius
	var dist_rot_y: float = (origin_2d + 0.7*0.707*(ox+oz) - at).length() - sensitivity_radius
	var dist_rot_z: float = (origin_2d + 0.7*0.707*(ox+oy) - at).length() - sensitivity_radius

	var draw: Dictionary = {
		origin = origin_2d, 
		axis_x = ox, 
		axis_y = oy, 
		axis_z = oz, 
		hover_x = (dist_x <= 0.0), 
		hover_y = (dist_y <= 0.0), 
		hover_z = (dist_z <= 0.0), 
		hover_rot_x = (dist_rot_x <= 0.0), 
		hover_rot_y = (dist_rot_y <= 0.0), 
		hover_rot_z = (dist_rot_z <= 0.0)
	}
	
	var axes: Dictionary = {
		axis_x = axis_x_w, 
		axis_y = axis_y_w, 
		axis_z = axis_z_w
	}
	
	return [draw, axes]




func _init_dragging( axis: Vector3 ):
	if target == null:
		return
	if _dragging.enabled:
		return
	
	_dragging.enabled = true
	
	# Own origin and unit vector.
	var t: Transform3D = target.transform

	var own_r: Vector3 = t.origin
	var own_a: Vector3 = axis #t.basis * (axis)
	
	_dragging.axis      = own_a
	_dragging.drag_axis = own_a
	_dragging.origin    = own_r
	_dragging.mouse_start = _mouse_on_axis( own_r, own_a )
	
	emit_signal( "drag_started", self )


func _process_dragging():
	_dragging.mouse_at = _mouse_on_axis( _dragging.mouse_start, _dragging.drag_axis )
	
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
	target.set_r( to )
	target.set_v( Vector3.ZERO )
	target.set_w( Vector3.ZERO )
	
	if r != _dragging.position:
		_dragging.position = r
		emit_signal( "position", r )



func _init_rotating( axis: Vector3 ):
	if target == null:
		return
	if _dragging.rot_enabled:
		return
	
	_dragging.rot_enabled = true
	
	# Own origin and unit vector.
	var t: Transform3D 
	var b: PhysicsBodyBase = target as PhysicsBodyBase
	if b != null:
		t = b.t()
	else:
		t = target.global_transform
	var axis_r: Vector3 = t.origin
	var axis_a: Vector3 = axis #t.basis * (axis)
	
	var mouse_crossing_r: Vector3 = _mouse_intersection_plane( axis_a, axis_r )
	# Going to drag along the line axis_a x (mouse_crossing_r - aixs_r).
	var drag_a: Vector3 = axis_a.cross( mouse_crossing_r - axis_r )
	drag_a = drag_a.normalized()
	
	
	
	var vp: SubViewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera3D = vp.get_camera_3d()
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	

	
	var q: Quaternion = t.basis
	
	var dist: float = (axis_r - cam_r).length()
	
	var viewport_width: float = vp.get_visible_rect().size.x
	_dragging.origin          = axis_r
	_dragging.drag_scale      = PI / dist
	_dragging.axis            = axis_a
	_dragging.drag_axis       = drag_a
	_dragging.start_q         = q
	_dragging.mouse_start     = mouse_crossing_r
	#print( "set mouse_start: ", _dragging.mouse_start )
	var euler: Vector3        = t.basis.get_euler()
	_dragging.euler = euler
	
	emit_signal( "drag_started", self )



func _process_rotating():
	_dragging.mouse_at = _mouse_on_axis( _dragging.mouse_start, _dragging.drag_axis )
	
	var dr: Vector3 = _dragging.mouse_at - _dragging.mouse_start
	var dx = dr.dot( _dragging.drag_axis )

	print( "axis: ", _dragging.drag_axis, "at: ", _dragging.mouse_at, " start: ", _dragging.mouse_start, " d: ", dr, " dot: ", dx )
	
	var angle: float = dx * _dragging.drag_scale
	
	var angle_2 = angle * 0.5
	var co2 = cos( angle_2 )
	var si2 = sin( angle_2 )
	var a: Vector3 = _dragging.axis
	var dq: Quaternion = Quaternion( a.x * si2, a.y * si2, a.z * si2, co2 )
	var q: Quaternion = dq * _dragging.start_q
	var b: Basis = q
	var euler: Vector3 = b.get_euler()
	euler.x = round( euler.x / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	euler.y = round( euler.y / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	euler.z = round( euler.z / Constants.CONSTRUCTION_ROT_SNAP ) * Constants.CONSTRUCTION_ROT_SNAP
	q = Quaternion( euler )

	#print( "rot_axis: ", a, "drag_axis: ", _dragging.drag_axis, ", mouse_at: ", _dragging.mouse_at, ", dr: ", dr, ", dot: ", dx, ", angle: ", angle, ", euler: ", euler )
	
	
	target.set_q( q )
	target.set_v( Vector3.ZERO )
	target.set_w( Vector3.ZERO )
	
	if euler != _dragging.euler:
		_dragging.euler = euler
		emit_signal( "euler", euler )



func _mouse_on_axis( mouse_start: Vector3, drag_axis: Vector3 ):
	var r = _mouse_intersection_line( drag_axis, mouse_start )
	# Now project this point onto the line.
	var ro: Vector3 = mouse_start
	var a: Vector3  = drag_axis
	
	var dr: Vector3 = r - ro
	var proj: float = a.dot( dr )
	var dp: Vector3 = a * proj
	var p: Vector3  = dp + ro
	
	return p


func _mouse_intersection_line( axis_a: Vector3, axis_r: Vector3 ):
	var vp: SubViewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera3D = vp.get_camera_3d()
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	var target_parent_rf: RefFrameNode     = target.get_parent()
	var cam_rf: RefFrameNode               = RootScene.ref_frame_root.player_camera
	var cam_rf_se3: Se3Ref                 = cam_rf.relative_to( target_parent_rf )
	var cam_to_target_parent_tr: Transform3D = cam_rf_se3.transform
	
	cam_r = cam_to_target_parent_tr * (cam_r)
	cam_a = cam_to_target_parent_tr.basis * (cam_a)
	
	var r: Vector3 = _line_crossing_point( cam_r, cam_a, axis_r, axis_a )
	
	#print( "mouse uv: ", mouse_uv, ", ro: ", cam_r, ", a: ", cam_a )
	return r



func _mouse_intersection_plane( plane_a: Vector3, plane_r: Vector3 ):
	var vp: SubViewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera3D = vp.get_camera_3d()
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	var target_parent_rf: RefFrameNode     = target.get_parent()
	var cam_rf: RefFrameNode               = RootScene.ref_frame_root.player_camera
	var cam_rf_se3: Se3Ref                 = cam_rf.relative_to( target_parent_rf )
	var cam_to_target_parent_tr: Transform3D = cam_rf_se3.transform
	
	cam_r = cam_to_target_parent_tr * (cam_r)
	cam_a = cam_to_target_parent_tr.basis * (cam_a)
	
	# r = plane_a.dot( cam_a * t + cam_r - plane_r ) = 0
	# t * dot(plane_a, cam_a) = dot(plane_a, plane_r - cam_r)
	var dot_plane_cam_a: float = plane_a.dot( cam_a )
	var dot_plane_cam_r: float = plane_a.dot( plane_r - cam_r )
	var t: float = dot_plane_cam_r / dot_plane_cam_a
	
	var r: Vector3 = cam_a * t + cam_r
	
	return r


func _line_crossing_point( r_a: Vector3, a_a: Vector3, r_b: Vector3, a_b: Vector3 ):
	var m: Basis
	m.x.x = 1.0 - a_a.x * a_a.x
	m.x.y = -a_a.y * a_a.x
	m.x.z = -a_a.z * a_a.x
	m.y.x = -a_a.x * a_a.y
	m.y.y = 1.0 - a_a.y * a_a.y
	m.y.z = -a_a.z * a_a.y
	m.z.x = -a_a.x * a_a.z
	m.z.y = -a_a.y * a_a.z
	m.z.z = 1.0 - a_a.z * a_a.z
	
	var a_A: Basis = m

	m.x.x = 1.0 - a_b.x * a_b.x
	m.x.y = -a_b.y * a_b.x
	m.x.z = -a_b.z * a_b.x
	m.y.x = -a_b.x * a_b.y
	m.y.y = 1.0 - a_b.y * a_b.y
	m.y.z = -a_b.z * a_b.y
	m.z.x = -a_b.x * a_b.z
	m.z.y = -a_b.y * a_b.z
	m.z.z = 1.0 - a_b.z * a_b.z
	
	var b_A: Basis = m
	
	var A: Basis
	A.x.x = b_A.x.x + a_A.x.x
	A.y.x = b_A.y.x + a_A.y.x
	A.z.x = b_A.z.x + a_A.z.x
	A.x.y = b_A.x.y + a_A.x.y
	A.y.y = b_A.y.y + a_A.y.y
	A.z.y = b_A.z.y + a_A.z.y
	A.x.z = b_A.x.z + a_A.x.z
	A.y.z = b_A.y.z + a_A.y.z
	A.z.z = b_A.z.z + a_A.z.z
	
	var inv_A: Basis = A.inverse()
	
	var p_a: Vector3 = a_A * (r_a)
	var p_b: Vector3 = b_A * (r_b)
	var p: Vector3 = p_b + p_a
	
	var r: Vector3 = inv_A * (p)
	
	#print( "mouse uv: ", mouse_uv, ", ro: ", cam_r, ", a: ", cam_a )
	return r



func _finit_dragging():
	_dragging.enabled = false
	emit_signal( "drag_finished", self )



func _finit_rotating():
	_dragging.rot_enabled = false
	emit_signal( "drag_finished", self )




func _on_world():
	use_local_space = false

func _on_local():
	use_local_space = true


