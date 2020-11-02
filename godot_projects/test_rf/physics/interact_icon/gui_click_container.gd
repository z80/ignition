
extends Panel


var _mouse = {
	down = false,
	in_title = false,
	down_pos = null,
	in_handle = false
}
var _pre_maximize_rect = get_rect()


func set_title( stri: String ):
	$TitleBar/Title.text = stri


# List of classes to instantiate objects.
func setup_gui( classes, target_object ):
	for C in classes:
		var inst = C.instance()
		$Container.add_child( inst )
		var has_init: bool = inst.has_method( "init" )
		if has_init:
			inst.init( target_object, self )
	
	var sz = $Container.rect_size
	print( "Container size: ", sz )
	for ch in $Container.get_children():
		sz = ch.rect_size
		print( "child size: ", sz )





func _ready():
	_pre_maximize_rect = get_rect()





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



func _on_Close_pressed():
	self.queue_free()
