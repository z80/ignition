extends Panel


var _time_scale_save: int = -1


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Save_pressed():
	pass # Replace with function body.


func _on_QuickSave_pressed():
	pass # Replace with function body.


func _on_Load_pressed():
	pass # Replace with function body.


func _on_Quit_pressed():
	pass # Replace with function body.




func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if key_pressed:
			return
		if event.scancode == KEY_ESCAPE:
			var v: bool = self.visible
			self.visible = not v
			if not v:
				_time_scale_save = PhysicsManager.get_time_scale()
				PhysicsManager.set_time_scale_0()
			
			else:
				PhysicsManager.set_time_scale( _time_scale_save )


