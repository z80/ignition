extends Control

# Text to visualize when mouse hovers.
var text: String setget _set_text, _get_text

var _mouse_over: bool = false

var _window = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass






func _set_text( stri: String ):
	text = stri
	$HoverGui.text = stri


func _get_text():
	return text



func _input( _event ):
	pass





func _on_mouse_entered():
	$HoverGui.visible = true
	_mouse_over = true


func _on_mouse_exited():
	$HoverGui.visible = false
	_mouse_over = false


func _on_MouseHoverControl_gui_input(event):
	if not _mouse_over:
		return
	if not (event is InputEventMouseButton):
		return
	var me: InputEventMouseButton = event as InputEventMouseButton
	# Open window on release event.
	if (me.button_index == BUTTON_LEFT) and (not me.pressed):
		var body: Body = get_parent() as Body
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
			
		var gui_classes = body.gui_classes()
		var GuiClickContainer = load( "res://physics/interact_icon/gui_click_container.tscn" )
		_window = GuiClickContainer.instance()
		body.add_child( _window )
		_window.setup_gui( gui_classes, body )
		
		var mouse_pos = get_viewport().get_mouse_position()
		_window.rect_position = mouse_pos
		
		# Also set this ref. frame as selected by the player.
		PhysicsManager.player_select = body





