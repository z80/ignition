
extends Part
class_name Decoupler

export(String) var decouple_sound = ""

# This one should point to the coupling node wich is iupposed to 
# be eliminated.
var decoupled: bool = false

# Called when the node enters the scene tree for the first time.
func init():
	.init()


func process_user_input_group( input: Dictionary ):
	.process_user_input_group( input )

	if input.has( "ui_space" ):
		var inp = input[ "ui_space" ]
		if inp.pressed:
			decoupler_activate()


# Eliminates upper coupling node. It causes two parts of the 
# rocket to separate.
func decoupler_activate():
	# Remove child connection.
	self.decouple()
	
	# Remove assembly.
	var sb: Node = get_super_body_raw()
	if (sb != null) and (is_instance_valid(sb)):
		sb.queue_free()
	
	if (not decoupled):
		play_sound( decouple_sound )
	
	decoupled = true


func serialize():
	var data: Dictionary = .serialize()
	data["decoupled"] = decoupled
	return data


func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	
	decoupled = data[ "decoupled" ]
	
	return true


