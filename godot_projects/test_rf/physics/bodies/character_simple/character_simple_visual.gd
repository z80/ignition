extends Spatial


var target_tps: Spatial = null
var target_fps: Spatial = null


# Called when the node enters the scene tree for the first time.
func _ready():
	target_tps = get_node("target_tps")
	target_fps = get_node("target_fps")


func set_speed( v: float ):
	if v < 0.0:
		v = 0.0
	elif v > 1.0:
		v = 1.0
	$AnimationTree.set( "parameters/blend_position", v )
