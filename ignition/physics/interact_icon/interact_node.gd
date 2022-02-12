extends Spatial


var target: Node = null setget _set_target, _get_target


func _set_target( t: Node ):
	target = t


func _get_target():
	return target


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
