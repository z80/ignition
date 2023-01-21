extends Spatial

var camera: Camera = null
var light: DirectionalLight = null

# Called when the node enters the scene tree for the first time.
func _ready():
	camera = get_node("Camera")
	light  = get_node("SunLight")


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
