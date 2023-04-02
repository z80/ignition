
extends Node3D

@export var sun_direction: Basis = Basis.IDENTITY: get = _get_sun_direction, set = _set_sun_direction

# This is actual root for adding visual content.
var root: Node3D = null

var _light: DirectionalLight3D = null


func get_light():
	var light: DirectionalLight3D = _get_light()
	return light



func _get_light():
	if _light == null:
		_light = get_node( "SunLight" )
	return _light


# Called when the node enters the scene tree for the first time.
func _ready():
	root = get_node( "Root" )


func _set_sun_direction( b: Basis ):
	_get_light()
	_light.transform.basis = b


func _get_sun_direction():
	_get_light()
	var ret: Basis = _light.transform.basis
	return ret


