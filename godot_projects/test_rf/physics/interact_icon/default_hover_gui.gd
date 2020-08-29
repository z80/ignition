extends Control


var text: String setget set_text, get_text


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func set_text( stri: String ):
	$Text.text = stri


func get_text():
	return $Text.text

