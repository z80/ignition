
extends CouplingNode
class_name CouplingNodeStacking

# The part can be connected to others via this node.
export(bool) var allows_connecting = true
# Other parts can be connected to this node.
export(bool) var allows_connections = true


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func try_couple_with( n: CouplingNode ):
	if not allows_connecting:
		return false
	
	var node_stacking: CouplingNodeStacking = n as CouplingNodeStacking
	if node_stacking == null:
		return false
	
	if not node_stacking.allows_connections:
		return false
	
	# n.owner.t * n.relative_to_owner = t * owner.t * relative_to_owner
	# t = n.owner.t * n.relative_to_owner * relative_to_woner.inverse() * owner.t.inverse()
	var n_owner_t: Transform = n.owner.transform
	var n_relative_to_owner: Transform = n.relative_to_owner
	var inv_relative_to_owner: Transform = relative_to_owner.inverse()
	var inv_owner_t: Transform = owner.transform
	var t: Transform = n_owner_t * n_relative_to_owner * inv_relative_to_owner * inv_owner_t
	
	base_transform = t
	part_b_path = n.get_path()
	part_b      = n
	is_parent   = false
	
	n.part_b_path = get_path()
	n.part_b      = self
	n.is_parent   = true
	
	return true





