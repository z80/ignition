extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var tree = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var og = OccupancyGrid.new()
	var rf = RefFrame.instance()
	var root = get_tree().get_root()
	root.add_child( rf )
	tree = RefFrameTree.new()
	tree.push_back( rf )
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
