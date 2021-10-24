
extends Node


var input: Dictionary = {}
var inputs: Array = [ "ui_w", "ui_s", "ui_a", "ui_d", "ui_q", "ui_e", 
					  "ui_z", "ui_x", "ui_c", "ui_v", 
					  "ui_i", "ui_k", "ui_j", "ui_l", "ui_u", "ui_o", "ui_m", "ui_space" ]

var gui_controls_new: Dictionary = {}
var gui_controls_active: Dictionary = {}
var gui_controls_to_remove: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _process( _delta ):
	for i in inputs:
		describe_event( i )
	
	# GUI controls
	_process_gui_controls()
	
	# Sending to all the recipients.
	#PhysicsManager.rpc( "user_input" )
	PhysicsManager.process_user_input( input )
	#print( "input: ", input )


func describe_event( e: String ):
	var is_pressed: bool       = Input.is_action_pressed( e )
	var is_just_pressed: bool  = Input.is_action_just_pressed( e )
	var is_just_released: bool = Input.is_action_just_released( e )
	
	# Add or replace.
	if is_pressed or is_just_pressed or is_just_released:
		var d = { pressed  = is_just_pressed, 
				  released = is_just_released }
		input[e] = d
	
	else:
		input.erase( e )


func gui_control_bool( id: String, pressed: bool, just_pressed: bool, just_released: bool ):
	var d: Dictionary = { pressed = just_pressed, released = just_released }
	gui_controls_new[ id ] = d



func get_input():
	return input


func _process_gui_controls():
	# Remove ones from previous list.
	for id in gui_controls_to_remove:
		var has: bool = gui_controls_active.has( id )
		if has:
			gui_controls_active.erase( id )
		has = input.has( id )
		if has:
			input.erase( id )
	gui_controls_to_remove.clear()
	# Add new ones and compose the deletion list.
	for id in gui_controls_new:
		var d: Dictionary = gui_controls_new[id]
		if d.released:
			gui_controls_to_remove.push_back( id )
		else:
			gui_controls_active[id] = d
	gui_controls_new.clear()
	
	# Update the dictionary which goes to the consumers.
	for id in gui_controls_active:
		var d: Dictionary = gui_controls_active[id]
		input[id] = d




