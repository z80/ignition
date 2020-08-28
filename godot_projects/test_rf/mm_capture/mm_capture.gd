extends Spatial


var capture = null


# Called when the node enters the scene tree for the first time.
func _ready():
	capture = OpenvrCaptureNode.new()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
