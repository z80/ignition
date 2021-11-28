
extends Control
class_name MenuBase

var menu_stack: Array = []

func push( NewMenu: PackedScene = null ):
	# Make sure that "self" presents in the stack.
	if menu_stack.empty():
		menu_stack.push_back( self )
		return
	
	var top: Control = menu_stack.back()
	if top != self:
		menu_stack.push_back( self )
	
	# Hide "self"
	self.visible = false
	
	# Create new menu and hide current one.
	var new_menu: Control = NewMenu.instance()
	var p: Control = self.get_parent()
	menu_stack.push_back( new_menu )
	new_menu.menu_stack = menu_stack
	p.add_child( new_menu )


func pop():
	if not menu_stack.empty():
		var top: Control = menu_stack.back()
		if top == self:
			menu_stack.pop_back()
	
	if not menu_stack.empty():
		var top: Control = menu_stack.back()
		top.return_back()
		
	self.queue_free()


func return_back():
	visible = true


func is_current():
	if menu_stack.empty():
		menu_stack.push_back( self )
		return
	
	var top: Control = menu_stack.back()
	var ret: bool = (top == self)
	return ret


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _input( event: InputEvent ):
	on_user_input( event )


func on_user_input( event: InputEvent ):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if key_pressed:
			return
		if event.scancode == KEY_ESCAPE:
			var current: bool = is_current()
			if current:
				pop()


