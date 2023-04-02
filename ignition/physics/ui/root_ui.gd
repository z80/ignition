extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_help_pressed():
	var Wiki = preload( "res://wiki_handler/wiki_window.tscn" )
	var wiki: Control = Wiki.instantiate()
	add_child( wiki )


func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if not key_pressed:
			return
		var key_code: int = event.keycode
		if key_code == KEY_F1:
			_on_help_pressed()
