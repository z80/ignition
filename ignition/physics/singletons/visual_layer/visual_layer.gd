
extends ViewportContainer

export(Basis) var sun_direction = Basis.IDENTITY setget _set_sun_direction, _get_sun_direction

# This is actual root for adding visual content.
var root: Spatial = null

var _viewport: Viewport = null
var _light: DirectionalLight = null
var _camera: Camera = null


# Called when the node enters the scene tree for the first time.
func _ready():
	return
	_viewport = get_node( "Viewport" )
	root = get_node( "Viewport/Root" )
	_camera   = root.camera
	_light    = root.light
	
	connect( "resized", self, "_on_resized" )


func _on_resized():
	var sz: Vector2 = self.get_rect().size
	_viewport.size = sz


func _set_sun_direction( b: Basis ):
	_light.transform.basis = b


func _get_sun_direction():
	var ret: Basis = _light.transform.basis
	return ret
