
extends CouplingNode
class_name CouplingNodeStacking

# The part can be connected to others via this node.
export(bool) var allows_connecting = true
# Other parts can be connected to this node.
export(bool) var allows_connections = true

var _joint: Generic6DOFJoint = null

# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func process():
	.process()
	position_rel_to_parent()


func couple_with( n: CouplingNode ):
	if not allows_connecting:
		return false
	
	var node_stacking: CouplingNodeStacking = n as CouplingNodeStacking
	if node_stacking == null:
		return false
	
	if not node_stacking.allows_connections:
		return false
	
	# Now measure the distance.
	var t_w: Transform = world_transform()
	var n_t_w: Transform = n.world_transform()
	var r_w: Vector3 = t_w.origin
	var n_r_w: Vector3 = n_t_w.origin
	var d: float = (r_w - n_r_w).length()
	
	var max_d: float = snap_size() + n.snap_size()
	if max_d < d:
		return false
	
	# No more excuses, couple these two together.
	node_b_path = n.get_path()
	node_b      = n
	is_parent   = false
	
	node_b.node_b_path = get_path()
	node_b.node_b      = self
	node_b.is_parent   = true
	
	return true


func decouple():
	if node_b_path == null:
		return
	node_b = get_node( node_b_path )
	if node_b == null:
		node_b_path = null
		return
	
	node_b.node_b_path = null
	node_b.node_b      = null
	node_b.is_parent   = false

	node_b_path = null
	node_b      = null
	is_parent   = false
	


func activate():
	var c: bool = connected()
	if not c:
		return
	if not is_parent:
		return
	_joint = Generic6DOFJoint.new()
	BodyCreator.root_node.add_child( _joint )
	
	var part_a: RefFrameNode = self.part
	var part_b: RefFrameNode = node_b.part
	
	var body_a: RigidBody = part_a._physical
	var body_b: RigidBody = part_b._physical
	
	var joint_t: Transform = self.world_transform()
	_joint.transform = joint_t
	_joint.set( "nodes/node_a", body_a.get_path() )
	_joint.set( "nodes/node_b", body_b.get_path() )
	_joint.precision = 100



func deactivate():
	if _joint == null:
		return
	
	_joint.queue_free()
	_joint = null




