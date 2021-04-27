
extends Spatial
class_name CouplingNode

enum NodeSize { SMALL=0, MEDIUM=1, LARGE=2 }
export(NodeSize) var size = NodeSize.MEDIUM

# If this part is attached to another one 
# here it is stored "parent" part path.
export(NodePath) var part_b_path = null
# The other s node is is connected to.
var part_b: CouplingNode = null
# If it was connected to, it's parent, else it's child.
var is_parent: bool = false

var relative_to_owner: Transform = Transform.IDENTITY

# This one attacheds to the surface.
# When it happens, some default transform is computed. 
# This transform is this one.
var base_transform: Transform = Transform.IDENTITY
# If this one is attached to other part 
# need to remember the rotation angle.
export(float) var angle: float = 0.0 setget _set_angle, _get_angle

# Angle only transform.
var angle_transform: Transform = Transform.IDENTITY




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



func _set_angle( a: float ):
	angle = a
	_update_angle_transform()



func _get_angle():
	return angle


func _update_angle_transform():
	var qx: Quat = Quat( Vector3.RIGHT, PI )
	var qy: Quat = Quat( Vector3.UP, angle )
	angle_transform = qy * qx


func compute_owner_rel_to_parent():
	_update_angle_transform()
	part_b = get_node( part_b_path )
	var t_cnr: Transform = relative_to_owner
	var inv_t_cnr: Transform = t_cnr.inverse()
	var t_a: Transform   = angle_transform
	var t_pnr: Transform
	if part_b != null:
		t_pnr = part_b.relative_to_owner
	else:
		t_pnr = Transform.IDENTITY
	var ret: Transform = t_pnr * t_a * inv_t_cnr
	return ret


