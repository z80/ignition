
extends Part
class_name Decoupler

@export var decouple_sound: String = ""

# This one should point to the coupling node wich is iupposed to 
# be eliminated.
var decoupled: bool = false

# Called when the node enters the scene tree for the first time.
func init():
	super.init()


func process_user_input_group( input: Dictionary ):
	super.process_user_input_group( input )

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
	var sb: Node = get_assembly_raw()
	if (sb != null) and (is_instance_valid(sb)):
		sb.queue_free()
	
	if (not decoupled):
		play_sound( decouple_sound )
	
	decoupled = true


func _serialize( data: Dictionary ):
	data["decoupled"] = decoupled
	return data


func _deserialize( data: Dictionary ):
	var ret: bool = super.deserialize( data )
	if not ret:
		return false
	
	decoupled = data[ "decoupled" ]
	
	return true


