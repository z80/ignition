extends ViewportContainer


export(Basis) var light_direction = Basis.IDENTITY setget _set_sun_direction, _get_sun_direction

var _light: DirectionalLight = null


func _ready():
	var a: int = 0
	var b: int = 1
	var c: int = 2


func _set_sun_direction( b: Basis ):
	_light.transform.basis = b


func _get_sun_direction():
	var ret: Basis = _light.transform.basis
	return ret


