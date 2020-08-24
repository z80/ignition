


var _mouse = {
	down = false,
	in_title = false,
	down_pos = null,
	in_handle = false
}



func _on_TitleBar_mouse_entered():
	_mouse.in_title = true

func _on_TitleBar_mouse_exited():
	_mouse.in_title = false

func _on_ResizeHandle_mouse_entered():
	_mouse.in_handle = true

func _on_ResizeHandle_mouse_exited():
	_mouse.in_handle = false

func _input(event):
	if(event is InputEventMouseButton):
		if(event.button_index == 1):
			_mouse.down = event.pressed
			if(_mouse.down):
				_mouse.down_pos = event.position

	if(_mouse.in_title):
		if(event is InputEventMouseMotion and _mouse.down):
			set_position(get_position() + (event.position - _mouse.down_pos))
			_mouse.down_pos = event.position
			_pre_maximize_rect = get_rect()

	if(_mouse.in_handle):
		if(event is InputEventMouseMotion and _mouse.down):
			var new_size = rect_size + event.position - _mouse.down_pos
			var new_mouse_down_pos = event.position
			rect_size = new_size
			_mouse.down_pos = new_mouse_down_pos
			_pre_maximize_rect = get_rect()

