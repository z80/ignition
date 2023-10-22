extends Control

@export var BuildGui: PackedScene = null
var _build_window: Control = null

@export var LocationQueryGui: PackedScene = null
var _location_query_window: Control = null



# Called when the node enters the scene tree for the first time.
func _ready():
	pass


func _on_build_pressed():
	if (_build_window == null) or (not is_instance_valid(_build_window) ):
		var build_window: Control = BuildGui.instantiate()
		add_child( build_window )
		_build_window = build_window
	


func _on_location_query_pressed():
	if (_location_query_window == null) or (not is_instance_valid(_location_query_window)):
		var lq: Control = LocationQueryGui.instantiate()
		add_child( lq )
		_location_query_window = lq
	


func _delete_dialogs():
	if (_build_window != null) and (is_instance_valid(_build_window)):
		_build_window.queue_free()
		_build_window = null

	if (_location_query_window != null) and (is_instance_valid(_location_query_window)):
		_location_query_window.queue_free()
		_location_query_window = null


func _input(event):
		if Input.is_action_just_pressed( "ui_open_build_dialog" ):
			_on_build_pressed()
		
		elif Input.is_action_just_pressed( "ui_location_dialog" ):
			_on_location_query_pressed()

		elif Input.is_action_just_pressed( "ui_escape" ):
			_delete_dialogs()



