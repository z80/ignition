
extends MenuBase

# By defaut
var _time_scale_save: int = RootScene.ref_frame_root.TimeScale.X_1

# Called when the node enters the scene tree for the first time.
func _ready():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Save_pressed():
	var SaveMenu: PackedScene = preload( "res://physics/ui/esc_menu/save_menu.tscn" )
	self.push( SaveMenu )
	UiSound.play( Constants.ButtonClick )



func _on_QuickSave_pressed():
	pass # Replace with function body.


func _on_Load_pressed():
	var LoadMenu: PackedScene = preload( "res://physics/ui/esc_menu/load_menu.tscn" )
	self.push( LoadMenu )
	UiSound.play( Constants.ButtonClick )


func _on_Quit_pressed():
	var QuitMenu: PackedScene = preload( "res://physics/ui/esc_menu/quit_menu.tscn" )
	self.push( QuitMenu )
	UiSound.play( Constants.ButtonClick )



func on_user_input( event: InputEvent ):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if key_pressed:
			return
		if event.scancode == KEY_ESCAPE:
			var v: bool = self.visible
			var current: bool = is_current()
			if current:
				if v:
					RootScene.ref_frame_root.set_time_scale( _time_scale_save )
					
				else:
					_time_scale_save = RootScene.ref_frame_root.get_time_scale()
					RootScene.ref_frame_root.set_time_scale_0()
				
				self.visible = not v


