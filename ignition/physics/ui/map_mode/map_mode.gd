extends Control



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Map_pressed():
	UiSound.play( Constants.ButtonClick )
	var btn: Button = get_node( "Map" )
	var pressed: bool = btn.pressed
	UserInput.gui_control_bool( "ui_map", pressed, pressed, not pressed )



func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if not key_pressed:
			return
		if event.scancode == KEY_M:
			UiSound.play( Constants.ButtonClick )
			var btn: Button = get_node( "Map" )
			var pressed: bool = btn.pressed
			pressed = not pressed
			btn.pressed = pressed
			UserInput.gui_control_bool( "ui_map", pressed, pressed, not pressed )



