
extends Part
class_name Decoupler

# This one should point to the coupling node wich is iupposed to 
# be eliminated.
export(NodePath) var decoupling_node_path


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func process_user_input_group( input: Dictionary ):
	.process_user_input_group( input )

	if input.has( "ui_space" ):
		var inp = input[ "ui_space" ]
		if inp.pressed:
			decoupler_activate()


# Eliminates upper coupling node. It causes two parts of the 
# rocket to separate.
func decoupler_activate():
	var decoupling_node: Node = get_node( decoupling_node_path )
	if (decoupling_node == null) or (not is_instance_valid(decoupling_node)):
		print( "ERROR: decoupling node is not specified" )
		return
	
	var has: bool = stacking_nodes.has( decoupling_node )
	if not has:
		print( "ERROR: decoupling node is not listed" )
		return
	
	#decoupling_node.d


func serialize():
	var data: Dictionary = .serialize()
	data["decoupling_node_path"] = String(decoupling_node_path)
	return data


func deserialize( data: Dictionary ):
	var ret: bool = .deserialize( data )
	if not ret:
		return false
	
	decoupling_node_path = data[ "decoupling_node_path" ]
	
	return true


