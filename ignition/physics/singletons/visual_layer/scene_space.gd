extends Node3D

@export var sun_direction: Basis = Basis.IDENTITY: get = _get_sun_direction, set = _set_sun_direction
@export var camera_global_rotation: Quaternion = Quaternion.IDENTITY: set = _set_global_rotation

var camera: Camera3D = null
var light: DirectionalLight3D = null
var background_sky: Node3D = null

# Called when the node enters the scene tree for the first time.
func _ready():
	camera = get_node("Camera3D")
	light  = get_node("SunLight")
	background_sky = get_node("BackgroundSky")


func _get_light():
	if light == null:
		light  = get_node("SunLight")


func _set_sun_direction( b: Basis ):
	_get_light()
	light.transform.basis = b


func _get_sun_direction():
	_get_light()
	return light.transform.basis


func _set_global_rotation( q: Quaternion ):
	camera_global_rotation = q.normalized()
	var inv_q: Quaternion = camera_global_rotation.inverse()
	var b: Basis = Basis( inv_q )
	
	var far: float = camera.far * 0.99
	b = b.scaled( Vector3( far, far, far ) )
	background_sky.transform.basis = b

