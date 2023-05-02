extends Node3D

@export var sun_direction: Basis = Basis.IDENTITY: get = _get_sun_direction, set = _set_sun_direction


var camera: Camera3D = null
var light: DirectionalLight3D = null

# Called when the node enters the scene tree for the first time.
func _ready():
	camera = get_node("Camera3D")
	light  = get_node("SunLight")


func _get_light():
	if light == null:
		light  = get_node("SunLight")


func _set_sun_direction( b: Basis ):
	_get_light()
	light.transform.basis = b


func _get_sun_direction():
	_get_light()
	return light.transform.basis


