extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var ref_frame = null
var ref_frame_tree = null

# Called when the node enters the scene tree for the first time.
func _ready():
	ref_frame = RefFrame.new()
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
