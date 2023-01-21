
extends Spatial

export(Basis) var sun_direction = Basis.IDENTITY setget _set_sun_direction, _get_sun_direction

# This is actual root for adding visual content.
var root: Spatial = null

#var _light: DirectionalLight = null
#var _camera: Camera = null


# Called when the node enters the scene tree for the first time.
func _ready():
	root = self




func _set_sun_direction( b: Basis ):
	#_light.transform.basis = b
	pass


func _get_sun_direction():
	#var ret: Basis = _light.transform.basis
	#return ret
	return Basis.IDENTITY


