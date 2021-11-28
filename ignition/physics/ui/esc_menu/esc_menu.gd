
extends MenuBase

# By defaut
var _time_scale_save: int = PhysicsManager.TimeScale.X_1

var _menu_stack: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	_menu_stack = []


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Save_pressed():
	var SaveMenu: PackedScene = preload( "res://physics/ui/esc_menu/save_menu.tscn" )
	self.push( SaveMenu )


func _on_QuickSave_pressed():
	pass # Replace with function body.


func _on_Load_pressed():
	pass # Replace with function body.


func _on_Quit_pressed():
	var t: SceneTree = get_tree()
	t.quit()



func on_user_input( event: InputEvent ):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if key_pressed:
			return
		if event.scancode == KEY_ESCAPE:
			if _menu_stack.empty():
				_menu_stack.push_back( self )
			
			var top: Control = _menu_stack.back()
			if top == self:
				var v: bool = self.visible
				if not v:
					_time_scale_save = PhysicsManager.get_time_scale()
					PhysicsManager.set_time_scale_0()
				
				else:
					PhysicsManager.set_time_scale( _time_scale_save )


