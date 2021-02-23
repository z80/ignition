
extends Node


var input: Dictionary = {}
var inputs: Array = [ "ui_w", "ui_s", "ui_a", "ui_d", "ui_q", "ui_e", 
					  "ui_z", "ui_x", "ui_c", "ui_v", 
					  "ui_i", "ui_k", "ui_j", "ui_l", "ui_u", "ui_o", "ui_m", "ui_space" ]

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _process( _delta ):
	for i in inputs:
		describe_event( i )
	
	#PhysicsManager.rpc( "user_input" )
	PhysicsManager.process_user_input( input )
	#print( "input: ", input )


func describe_event( e: String ):
	var is_pressed: bool = Input.is_action_pressed( e )
	var is_just_pressed: bool = Input.is_action_just_pressed( e )
	var is_just_released: bool = Input.is_action_just_released( e )
	
	# Add or replace.
	if is_pressed or is_just_pressed or is_just_released:
		var d = { pressed = is_just_pressed, 
				 released = is_just_released}
		input[e] = d
	
	else:
		input.erase( e )
