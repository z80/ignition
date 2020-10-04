extends CanvasItem


export(NodePath) var target_path setget set_path
var target: Spatial = null

# size dimensions.
export(int) var axis_length = 32
export(int) var sensitivity_radius = 6

const COLOR_X: Color = Color( 0.7, 0.0, 0.0, 0.0 )
const COLOR_Y: Color = Color( 0.0, 0.7, 0.0, 0.0 )
const COLOR_Z: Color = Color( 0.0, 0.0, 0.7, 0.0 )

const COLOR_X_HOVER: Color = Color( 1.0, 0.0, 0.0, 0.0 )
const COLOR_Y_HOVER: Color = Color( 0.0, 1.0, 0.0, 0.0 )
const COLOR_Z_HOVER: Color = Color( 0.0, 0.0, 1.0, 0.0 )

func set_path( path ):
	target_path = path
	target = get_node( target_path )

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _draw():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func get_draw_params():
	if target == null:
		return null
	var vp: Viewport = get_viewport()
	if vp == null:
		return null
	var cam = vp.get_camera()
	if cam == null:
		return null
	
	var t_target: Transform = target.global_transform
	var t_camera: Transform = cam.global_transform
	
	var t: Transform = t_camera.inverse() * t_target
	
	var uv: Vector2 = cam.unproject_position( t_target.origin )
	var uv_x: Vector2 = cam.unproject_position( t_target.origin + Vector3( 1.0, 0.0, 0.0 ) ) - uv
	var l: float = uv_x.length()
	if l > 0.001:
		uv_x = uv_x.normalized()
	var uv_y: Vector2 = cam.unproject_position( t_target.origin + Vector3( 0.0, 1.0, 0.0 ) ) - uv
	l = uv_y.length()
	if l > 0.001:
		uv_y = uv_y.normalized()
	var uv_z: Vector2 = cam.unproject_position( t_target.origin + Vector3( 0.0, 0.0, 1.0 ) ) - uv
	l = uv_z.length()
	if l > 0.001:
		uv_z = uv_z.normalized()
	
	var res = [uv, uv_x, uv_y, uv_z]
	return res
	
