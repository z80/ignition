
extends Spatial
class_name CharacterVisualBase

var target_tps: Spatial = null
var target_fps: Spatial = null


# Called when the node enters the scene tree for the first time.
func _ready():
	target_tps = get_node("target_tps")
	target_fps = get_node("target_fps")
	set_speed_normalized( 0.0, Vector3.ZERO )


# This one should be overridden.
func set_speed_normalized( _s: float, _v: Vector3 ):
	pass



