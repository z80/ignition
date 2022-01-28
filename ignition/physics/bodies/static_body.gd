
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
	if _first_parent != null:
		return
	var ClosestCb = preload( "res://physics/utils/closest_celestial_body.gd" )
	var cb: CelestialBody = ClosestCb.closest_celestial_body( self )
	var cs: CelestialSurface = cb as CelestialSurface


func _update_position():
	pass



