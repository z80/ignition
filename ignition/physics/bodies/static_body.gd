
extends PhysicsBodyBase
class_name StaticPhysicsBody

var _first_parent: RefFrameNode = null
var _first_se3: Se3Ref          = null

func get_class():
	return "StaticPhysicsBody"


func _notification( what ):
	if (what == NOTIFICATION_PARENTED) or (what == NOTIFICATION_PATH_CHANGED):
		if (_first_se3 == null) or ( not is_instance_valid(_first_parent) ):
			_init_first_parent()
		
		else:
			_update_position()



func _init_first_parent():
	pass


func _update_position():
	pass



