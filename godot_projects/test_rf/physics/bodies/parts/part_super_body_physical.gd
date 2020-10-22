extends Node
class_name PartSuperBodyPhysical

var _part_super_body = null
var _joints: Array = []

# Just a placeholder to make it compatible.
var transform: Transform

# Just a placeholder to make it compatible.
func set_linear_velocity( v ):
	pass

# Just a placeholder to make it compatible.
func set_angular_velocity( w ):
	pass

# Just a placeholder to make it compatible.
func set_collision_layer( bit ):
	pass

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func setup( bodies: Array, edges: Array ):
	print( "PartSuperBodyPhysical::setup()" )
	for joint in _joints:
		joint.queue_free()
	_joints.clear()
	
	# Create joints.
	for edge in edges:
		var ind_a: int = edge[0]
		var ind_b: int = edge[1]
		var body_a = bodies[ind_a]
		var body_b = bodies[ind_b]
		
		var j = Generic6DOFJoint.new()
		BodyCreator.root_node.add_child( j )
		_joints.push_back( j )
		
		j.set( "nodes/node_a", body_a._physical.get_path() )
		j.set( "nodes/node_b", body_b._physical.get_path() )
		j.precision = 10



func destroy():
	print( "PartSuperBodyPhysical::destroy()" )
	var qty = _joints.size()
	for i in range( qty ):
		var joint = _joints[i]
		joint.queue_free()
	
	_joints.clear()


func _exit_tree():
	print( "PartSuperBodyPhysical::exit_tree()" )
	destroy()



