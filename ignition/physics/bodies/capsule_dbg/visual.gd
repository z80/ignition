extends Spatial


var target_tps: Spatial = null
var target_fps: Spatial = null


# Called when the node enters the scene tree for the first time.
func _ready():
	target_tps = get_node("target_tps")
	target_fps = get_node("target_fps")


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
