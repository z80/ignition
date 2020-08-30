extends Spatial


export(NodePath) var target_path setget _set_target_path
var target = null


var _mouse = {
	hover_x = false, 
	hover_y = false, 
	hover_z = false
}

var _dragging = {
	enabled = false, 
	axis = Vector3.ZERO, 
	origin = Vector3.ZERO, 
	mouse_start = Vector3.ZERO, 
	mouse_at = Vector3.ZERO
}


# Called when the node enters the scene tree for the first time.
func _ready():
	_set_target_path( target_path )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process( delta ):
	if _dragging.enabled:
		_process_dragging()


func _set_target_path( t ):
	target_path = t
	target = get_node( t )


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


func _process_input( event ):
	var me: InputEventMouseButton = event as InputEventMouseButton
	if not me:
		return
	if (me.button_index != BUTTON_LEFT):
		return
	
	_dragging.enabled = me.pressed
	
	if _dragging.enabled:
		if _mouse.hover_x:
			_init_dragging( Vector3.RIGHT )
		elif _mouse.hover_y:
			_init_dragging( Vector3.UP )
		else:
			_init_dragging( Vector3.BACK )
	
	else:
		_finit_dragging()



func _init_dragging( axis: Vector3 ):
	# Own origin and unit vector.
	var t: Transform = self.global_transform
	var own_r: Vector3 = t.origin
	var own_a: Vector3 = t.basis.xform( axis )
	
	_dragging.axis   = own_a
	_dragging.origin = own_r
	_dragging.mouse_start = _mouse_on_axis()


func _process_dragging():
	_dragging.mouse_at = _mouse_on_axis()
	var sz: float = Constants.CONSTRUCTION_GRID_SZ
	var x: float = _dragging.mouse_at.x / sz
	var y: float = _dragging.mouse_at.y / sz
	var z: float = _dragging.mouse_at.z / sz
	x = round( x ) * sz
	y = round( y ) * sz
	z = round( z ) * sz
	
	# Here probably need to convert back to local 
	# first as all vectors are in global ref frame
	self.translation = Vector3( x, y, z )
	if target != null:
		target.translation = self.translation


func _mouse_on_axis():
	var r = _mouse_intersection()
	# Now project this point onto the line.
	var ro: Vector3 = _dragging.origin
	var a: Vector3  = _dragging.axis
	
	var dr: Vector3 = r - ro
	var proj: float = a.dot( dr )
	var dp: Vector3 = a * proj
	var p: Vector3  = dp + ro
	
	return p


func _mouse_intersection():
	var vp: Viewport = get_viewport()
	var mouse_uv = vp.get_mouse_position()
	
	var camera: Camera = PhysicsManager.camera
	
	# Camera origin and unit vector.
	var cam_r: Vector3 = camera.project_ray_origin(mouse_uv)
	var cam_a: Vector3 = camera.project_ray_normal(mouse_uv)
	
	# Axes origin and unit vector
	var own_a: Vector3 = _dragging.axis
	var own_r: Vector3 = _dragging.origin
	
	var m: Basis
	m.x.x = 1.0 - own_a.x * own_a.x
	m.x.y = own_a.y * own_a.x
	m.x.z = own_a.z * own_a.x
	m.y.x = own_a.x * own_a.y
	m.y.y = 1.0 - own_a.y * own_a.y
	m.y.z = own_a.z * own_a.y
	m.z.x = own_a.x * own_a.z
	m.z.y = own_a.y * own_a.z
	m.z.z = 1.0 - own_a.z * own_a.z
	
	var own_A: Basis = m

	m.x.x = 1.0 - cam_a.x * cam_a.x
	m.x.y = cam_a.y * cam_a.x
	m.x.z = cam_a.z * cam_a.x
	m.y.x = cam_a.x * cam_a.y
	m.y.y = 1.0 - cam_a.y * cam_a.y
	m.y.z = cam_a.z * cam_a.y
	m.z.x = cam_a.x * cam_a.z
	m.z.y = cam_a.y * cam_a.z
	m.z.z = 1.0 - cam_a.z * cam_a.z
	
	var cam_A: Basis = m
	
	var A: Basis
	A.x.x = own_A.x.x + cam_A.x.x
	A.x.y = own_A.x.y + cam_A.x.y
	A.x.z = own_A.x.z + cam_A.x.z
	A.y.x = own_A.y.x + cam_A.y.x
	A.y.y = own_A.y.y + cam_A.y.y
	A.y.z = own_A.y.z + cam_A.y.z
	A.z.x = own_A.z.x + cam_A.z.x
	A.z.y = own_A.z.y + cam_A.z.y
	A.z.z = own_A.z.z + cam_A.z.z
	
	var inv_A: Basis = A.inverse()
	
	var own_p: Vector3 = own_A.xform( own_r )
	var cam_p: Vector3 = cam_A.xform( cam_r )
	var p: Vector3 = own_p + cam_p
	
	var r: Vector3 = inv_A.xform( p )
	
	return r


func _finit_dragging():
	emit_signal( "finished" )



