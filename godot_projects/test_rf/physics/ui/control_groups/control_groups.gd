extends Panel


var _msgs: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	var msgs: Array = [
		[KEY_1, "gui_group_1", get_node("VBox/Group1")], 
		[KEY_2, "gui_group_2", get_node("VBox/Group2")], 
		[KEY_3, "gui_group_3", get_node("VBox/Group3")], 
		[KEY_4, "gui_group_4", get_node("VBox/Group4")], 
		[KEY_5, "gui_group_5", get_node("VBox/Group5")], 
	]
	_msgs = msgs


func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if not key_pressed:
			return
		for msg in _msgs:
			var key = msg[0]
			if event.scancode == key:
				var check = msg[2]
				var pressed: bool = check.pressed
				pressed = not pressed
				check.pressed = pressed
				var gui_input_stri: String = msg[1]
				UserInput.gui_control_bool( gui_input_stri, pressed, pressed, not pressed )
				check.pressed = pressed




func _on_group_pressed():
	for msg in _msgs:
		var check = msg[2]
		var pressed: bool = check.pressed
		var gui_input_stri: String = msg[1]
		UserInput.gui_control_bool( gui_input_stri, pressed, pressed, not pressed )



