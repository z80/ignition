
extends Node

@export var sun_direction: Basis = Basis.IDENTITY: get = _get_sun_direction, set = _set_sun_direction

# This is actual root for adding visual content.
var root: Node3D = null

var _viewport: SubViewport = null
var _light: DirectionalLight3D = null
var _camera: Camera3D = null


func get_viewport():
	if _viewport == null:
		_viewport = get_node( "SubViewport" )
	return _viewport


func get_light():
	var light: DirectionalLight3D = _get_light()
	return light


func _get_light():
	if _light == null:
		_light = get_node( "SubViewport/Root/SunLight" )
	return _light



# Called when the node enters the scene tree for the first time.
func _ready():
	root = get_node( "SubViewport/Root" )
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
