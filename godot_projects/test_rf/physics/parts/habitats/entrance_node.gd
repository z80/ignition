
extends Spatial
class_name EntranceNode

var relative_to_owner: Transform = Transform.IDENTITY

enum NodeSize { SMALL=0, MEDIUM=1, LARGE=2 }
export(NodeSize) var node_size = NodeSize.MEDIUM


# Called when the node enters the scene tree for the first time.
func _ready():
	relative_to_owner = compute_relative_to_owner()


func compute_relative_to_owner():
	var p: Node = self
	var t: Transform = Transform.IDENTITY
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret



func _compute_relative_to_owner_recursive( n: Node, t: Transform ):
	var s: Spatial = n as Spatial
	if s != null:
		var ct: Transform = s.transform
		t = t * ct
	var ow: Node = self.owner
	if n == ow:
		return t
	
	var p: Node = get_parent()
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret


func compute_node_size():
	if node_size == NodeSize.SMALL:
		return Constants.NODE_SIZE_SMALL
	elif node_size == NodeSize.MEDIUM:
		return Constants.NODE_SIZE_MEDIUM
	
	return Constants.NODE_SIZE_LARGE



