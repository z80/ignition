extends Control


var _controls: Dictionary = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	_controls = {}
	_controls[KEY_F5] = { node=get_node( "Pause" ),  method="set_time_scale_0" }
	_controls[KEY_F6] = { node=get_node( "Lower" ),  method="set_time_scale_lower" }
	_controls[KEY_F7] = { node=get_node( "Normal" ), method="set_time_scale_1" }
	_controls[KEY_F8] = { node=get_node( "Higher" ), method="set_time_scale_higher" }
	
	# Connect to methods.
	for key in _controls:
		var data: Dictionary = _controls[key]
		var node: Node     = data.node
		var method: String = data.method
		node.connect( "pressed", self, method )


func _input(event):
	if (event is InputEventKey) and (not event.echo):
		var key_pressed: bool = event.pressed
		#print( "pressed: ", pressed )
		if not key_pressed:
			return
		var key_code: int = event.scancode
		var has: bool = _controls.has( key_code )
		if not has:
			return
		
		var data: Dictionary = _controls[key_code]
		var method: String = data.method
		self.call( method )


func set_time_scale_0():
	UiSound.play( Constants.ButtonClick )
	RootScene.ref_frame_root.set_time_scale_0()
	_set_time_scale_text()


func set_time_scale_lower():
	UiSound.play( Constants.ButtonClick )
	RootScene.ref_frame_root.decrease_time_scale()
	_set_time_scale_text()


func set_time_scale_1():
	UiSound.play( Constants.ButtonClick )
	RootScene.ref_frame_root.set_time_scale_1()

	_set_time_scale_text()


func set_time_scale_higher():
	UiSound.play( Constants.ButtonClick )
	RootScene.ref_frame_root.increase_time_scale()
	_set_time_scale_text()



func _set_time_scale_text():
	var l: Label = get_node( "Label" )
	l.text = RootScene.ref_frame_root.get_time_scale_string()





