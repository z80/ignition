
extends Spatial
class_name PartNode

enum NodeSize { SMALL=0, MEDIUM=1, LARGE=2 }
export(NodeSize) var size = NodeSize.MEDIUM

# If this part is attached to another one 
# here it is stored "parent" part path.
export(NodePath) var part_b_path = null
# Parent path pointer for faster access.
var parent_part: RefFrameNode = null
# If this one is attached to other part 
# need to remember the rotation angle.
var angle: float = 0.0

var relative_to_owner: Transform = Transform.IDENTITY



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







