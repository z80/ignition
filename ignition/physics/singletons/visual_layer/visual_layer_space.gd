
extends Node

export(Basis) var sun_direction = Basis.IDENTITY setget _set_sun_direction, _get_sun_direction

# This is actual root for adding visual content.
var root: Spatial = null

var _viewport: Viewport = null
var _light: DirectionalLight = null
var _camera: Camera = null


func get_viewport():
	if _viewport == null:
		_viewport = get_node( "Viewport" )
	return _viewport


func get_light():
	var light: DirectionalLight = _get_light()
	return light


func _get_light():
	if _light == null:
		_light = get_node( "Viewport/Root/SunLight" )
	return _light



# Called when the node enters the scene tree for the first time.
func _ready():
	root = get_node( "Viewport/Root" )
	_camera   = root.camera
	_light    = root.light



func _on_resized():
	var sz: Vector2 = get_tree().get_viewport().size #self.get_rect().size
	get_viewport()
	_viewport.size = sz


func _set_sun_direction( b: Basis ):
	_get_light()
	_light.transform.basis = b


func _get_sun_direction():
	_get_light()
	var ret: Basis = _light.transform.basis
	return ret
