
extends Node
class_name CouplingAttachment

# Which part it belongs to.
var part: RefFrameNode = null
# The other node's connection is is connected to.
var attachment_b: CouplingAttachment = null
# If it was connected to, it's parent, else it's child.
var is_parent: bool = false

# This one attacheds to the surface.
# When it happens, some default transform is computed. 
# This transform is this one.
export(Transform) var base_transform = Transform.IDENTITY
# And this transform is the result of "base_transform" plus rotation_angles.
export(Transform) var transform = Transform.IDENTITY
# If this one is attached to other part 
# need to remember the rotation angle.
export(float) var yaw: float = 0.0 setget _set_yaw, _get_yaw
export(float) var pitch: float = 0.0 setget _set_pitch, _get_pitch
export(float) var roll: float = 0.0 setget _set_roll, _get_roll

var _joint: Generic6DOFJoint = null



func init( own_part: RefFrameNode, base_t: Transform, it_is_parent: bool ):
	part           = own_part
	base_transform = base_t
	is_parent      = it_is_parent


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _exit_tree():
	var to_be_deleted: bool = is_queued_for_deletion()
	if to_be_deleted:
		on_delete()


func on_delete():
	deactivate()



func get_part():
	var p: Node = get_parent()
	var part: RefFrameNode = p as RefFrameNode
	return part



func _set_yaw( v: float ):
	yaw = v

func _get_yaw():
	return yaw

func _set_pitch( v: float ):
	pitch = v

func _get_pitch():
	return pitch

func _set_roll( v: float ):
	roll = v

func _get_roll():
	return roll


#func update_joint_pose( t_owner: Transform ):
#	if _joint != null:
#		var t: Transform = t_owner * relative_to_owner
#		_joint.transform = t




func get_part_transform():
	if part == null:
		return Transform.IDENTITY
	var se3: Se3Ref = part.get_se3()
	var t: Transform = se3.transform
	return t


func get_angles_transform():
	var yaw_q: Quat   = Quat( Vector3.RIGHT, yaw )
	var pitch_q: Quat = Quat( Vector3.UP, yaw )
	var roll_q: Quat  = Quat( Vector3.BACK, yaw )
	var q: Quat = yaw_q * pitch_q * roll_q
	var t: Transform = Transform( Basis(q), Vector3.ZERO )
	return t


func world_transform():
	var part_t: Transform = get_part_transform()
	var base_t: Transform = base_transform
	var ang_t: Transform  = get_angles_transform()
	var t: Transform = part_t * base_t * ang_t
	return t



func compute_part_rel_to_parent_part():
	var t_cnr: Transform = base_transform
	var inv_t_cnr: Transform = t_cnr.inverse()
	var t_pnr: Transform
	if attachment_b != null:
		t_pnr = attachment_b.base_transform
	else:
		t_pnr = Transform.IDENTITY
	var t_a: Transform = get_angles_transform()
	var ret: Transform = t_pnr * t_a * inv_t_cnr
	return ret



# When connected position it's owner correctly with respecto to the parent.
func position_rel_to_parent():
	if is_parent:
		return false
	
	var t_rel: Transform = compute_part_rel_to_parent_part()
	var parent_part: RefFrameNode = attachment_b.get_part()
	if not is_instance_valid(parent_part):
		return
	var t_parent: Transform = parent_part.get_se3().transform
	var t: Transform = t_parent * t_rel
	part.transform = t




func activate():
	if not is_parent:
		return
	
	var part_a: RefFrameNode = self.get_part()
	var part_b: RefFrameNode = attachment_b.get_part()
	
	var body_a: RigidBody = part_a._physical
	var body_b: RigidBody = part_b._physical
	
	assert( is_instance_valid(body_a) )
	assert( is_instance_valid(body_b) )
	
	_joint = Generic6DOFJoint.new()
	var physics_env = body_a.get_parent()
	physics_env.add_child( _joint )
	
	var joint_t: Transform = world_transform()
	_joint.transform = joint_t
	_joint.set( "nodes/node_a", body_a.get_path() )
	_joint.set( "nodes/node_b", body_b.get_path() )
	#_joint.precision = 100
	_joint.set( "linear_limit_x/softness", 0.00001 )
	_joint.set( "linear_limit_y/softness", 0.00001 )
	_joint.set( "linear_limit_z/softness", 0.00001 )



func deactivate():
	if _joint == null:
		return
	
	if is_parent:
		_joint.queue_free()
		attachment_b.deactivate()
	_joint = null





func serialize():
	var data: Dictionary = {}
	data["yaw"]        = yaw
	data["pitch"]      = pitch
	data["roll"]       = roll
	
	data["is_parent"]  = is_parent
	data["coupling_b"] = attachment_b.get_path()
	return data


func deserialize( own_part: RefFrameNode, data: Dictionary ):
	part = own_part
	
	var path: String = data["coupling_b"]
	attachment_b = part.get_node( path )
	is_parent  = data["is_parent"]
	
	yaw       = data["yaw"]
	pitch     = data["pitch"]
	roll      = data["roll"]
	
	return true


