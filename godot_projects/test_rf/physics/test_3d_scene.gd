extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var v = preload( "res://physics/bodies/cube/visual.tscn" ).instance()
	var tree = get_tree()
	var root = tree.get_root()
	root.add_child( v )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
