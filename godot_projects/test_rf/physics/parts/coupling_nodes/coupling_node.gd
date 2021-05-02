
extends Spatial
class_name CouplingNode

enum NodeSize { SMALL=0, MEDIUM=1, LARGE=2 }
export(NodeSize) var node_size = NodeSize.MEDIUM

# Reference to the part.
# Can't use "owner" field as it shows the Visual's root which 
# isn't the part itself.
var part: RefFrameNode = null

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
#var base_transform: Transform = Transform.IDENTITY
# If this one is attached to other part 
# need to remember the rotation angle.
export(float) var angle: float = 0.0 setget _set_angle, _get_angle

# Angle only transform.
#var angle_transform: Transform = Transform.IDENTITY

# Fordisplaying visuals.
export(bool) var show_visual = false setget _set_show_visual, _get_show_visual
var _visual = null



# Called when the node enters the scene tree for the first time.
func _ready():
	relative_to_owner = compute_relative_to_owner()


# This thing should be called when in kinematic mode.
# Is should position child parts relatie to parent parts in the right way.
# Should be overwritten in derived classes.
func process():
	if show_visual and (_visual != null):
		var t: Transform = world_transform()
		_visual.transform = t
		_visual.size = snap_size()



func connected():
	var ret: bool = (part_b != null) and is_instance_valid(part_b)
	return ret



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



func _get_angle():
	return angle



func _set_show_visual( en: bool ):
	show_visual = en
	if show_visual:
		if _visual == null:
			var Visual = preload( "res://physics/parts/coupling_nodes/coupling_node_visual.tscn" )
			_visual = Visual.instance()
			_visual.size = snap_size()
			BodyCreator.root_node.add_child( _visual )
		_visual.visible = true
	else:
		if _visual != null:
			_visual.queue_free()
			_visual = null



func _get_show_visual():
	return show_visual



func _compute_angle_transform():
	var qx: Quat = Quat( Vector3.RIGHT, PI )
	var qy: Quat = Quat( Vector3.UP, angle )
	var q: Quat = qy * qx
	var t: Transform = Transform.IDENTITY
	t.basis = q
	return t


func compute_owner_rel_to_parent():
	part_b = get_node( part_b_path )
	var t_cnr: Transform = relative_to_owner
	var inv_t_cnr: Transform = t_cnr.inverse()
	var t_pnr: Transform
	if part_b != null:
		t_pnr = part_b.relative_to_owner
	else:
		t_pnr = Transform.IDENTITY
	var t_a: Transform = _compute_angle_transform()
	var ret: Transform = t_pnr * t_a * inv_t_cnr
	return ret


# When connected position it's owner correctly with respecto to the parent.
func position_rel_to_parent():
	part_b = get_node( part_b_path )
	if part_b == null:
		return false
	if is_parent:
		return false
	var t_rel: Transform = compute_owner_rel_to_parent()
	var parent_rf: RefFrameNode = part_b.part
	if parent_rf == null:
		return
	var t_parent: Transform = parent_rf.transform
	var t: Transform = t_parent * t_rel
	var rf: RefFrameNode = self.part
	rf.transform = t


func world_transform():
	var parent_rf: RefFrameNode = part_b.part
	var t_parent: Transform = parent_rf.transform
	var t: Transform = t_parent * relative_to_owner
	return t


func snap_size():
	if node_size == NodeSize.SMALL:
		return Constants.NODE_SIZE_SMALL
	elif node_size == NodeSize.MEDIUM:
		return Constants.NODE_SIZE_MEDIUM
	
	return Constants.NODE_SIZE_LARGE







