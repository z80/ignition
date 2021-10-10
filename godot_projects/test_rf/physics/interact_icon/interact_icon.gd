extends Control

# Should be set externally.
var body: Node = null

# Text to visualize when mouse hovers.
var text: String setget _set_text, _get_text

var _mouse_over: bool = false

var _window = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var body_exists: bool = is_instance_valid( body )
	if not body_exists:
		self.queue_free()






func _set_text( stri: String ):
	text = stri
	self.hint_tooltip = stri


func _get_text():
	return text



func _input( _event ):
	pass





func _on_mouse_entered():
	_mouse_over = true


func _on_mouse_exited():
	_mouse_over = false


func _on_mouse_gui_input(event):
	if not _mouse_over:
		return
	if not (event is InputEventMouseButton):
		return
	var me: InputEventMouseButton = event as InputEventMouseButton
	print( "here, ", me.pressed )
	# Open window on release event.
	if (me.button_index == BUTTON_LEFT) and (not me.pressed):
		if body == null:
			return
		
		var has_show_click_container = body.has_method( "show_click_container" )
		if not has_show_click_container:
			return
		var do_show_container: bool = body.show_click_container()
		if not do_show_container:
			return
		
		# If was deleted clear the reference.
		if not is_instance_valid( _window ):
			_window = null
		
		if _window != null:
			_window.visible = true
			return
		
		var mode: Array = body.gui_mode()
		var gui_classes = body.gui_classes( mode )
		var GuiClickContainer = load( "res://physics/interact_icon/gui_click_container.tscn" )
		_window = GuiClickContainer.instance()
		var parent_for_windows: Control = RootScene.get_root_for_gui_windows()
		parent_for_windows.add_child( _window )
		_window.setup_gui( gui_classes, body )
		
		var mouse_pos = get_viewport().get_mouse_position()
		_window.rect_position = mouse_pos
		
		# Also set this ref. frame as selected by the player.
		PhysicsManager.player_select = body





