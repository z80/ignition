extends Node3D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _init():
	print( "AxesVisualization::_init(): ", self )


func _leave_tree():
	if is_queued_for_deletion():
		print( "AxesVisualization::on_delete(), ", self )

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
