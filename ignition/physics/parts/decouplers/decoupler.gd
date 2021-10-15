
extends Part
class_name Decoupler

# This one should point to the coupling node wich is iupposed to 
# be eliminated.
var decoupling_node: Spatial = null
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
	if (decoupling_node == null) or (not is_instance_valid(decoupling_node)):
		print( "ERROR: decoupling node is not specified" )
		return
	
	var has: bool = stacking_nodes.has( decoupling_node )
	if not has:
		print( "ERROR: decoupling node is not listed" )
		return
	
	# Remove assembly.
	var sb: Node = get_super_body_raw()
	if (sb != null) and (is_instance_valid(sb)):
		sb.queue_free()
	
	
	var node_b: CouplingNodeStacking = decoupling_node.node_b
	if node_b != null:
		node_b.deactivate()
		node_b.node_b_path = null
		node_b.node_b      = null
		node_b.is_parent   = false
	decoupling_node.deactivate()
	decoupling_node.node_b_path = null
	decoupling_node.node_b      = null
	decoupling_node.is_parent   = false
	
	# Remove decoupling node from the list of stacking nodes.
	stacking_nodes.erase( decoupling_node )
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
	if decoupled:
		stacking_nodes.erase( decoupling_node )
	
	return true


