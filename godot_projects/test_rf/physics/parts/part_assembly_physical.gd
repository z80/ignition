extends Node
class_name PartAssemblyPhysical

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



