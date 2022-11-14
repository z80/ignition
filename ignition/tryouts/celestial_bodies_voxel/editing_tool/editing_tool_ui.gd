extends Control


var surfaces: Node = null


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Update_toggled(button_pressed):
	if surfaces != null:
		surfaces.update_disabled = button_pressed


func _on_Wireframe_toggled(button_pressed):
	if surfaces != null:
		surfaces.wireframe = button_pressed
