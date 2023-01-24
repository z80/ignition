extends Spatial

export(Basis) var sun_direction = Basis.IDENTITY setget _set_sun_direction, _get_sun_direction


var camera: Camera = null
var light: DirectionalLight = null

# Called when the node enters the scene tree for the first time.
func _ready():
	camera = get_node("Camera")
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


