
extends PhysicsBodyBase
class_name StaticPhysicsBody

var _first_parent: RefFrameNode = null
var _first_se3: Se3Ref          = null

#func get_class():
#	return "StaticPhysicsBody"



func _notification( what ):
	if (what == NOTIFICATION_PARENTED) or (what == NOTIFICATION_PATH_RENAMED):
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
	
	if cs == null:
		return
	
	_first_parent = cs
	_first_se3    = self.relative_to( cs )



func _update_position():
	if (_first_parent == null) or (_first_se3 == null):
		return
	
	var parent: RefFrameNode = self.get_parent() as RefFrameNode
	if parent == null:
		return
	
	var se3: Se3Ref = _first_parent.se3_relative_to( _first_se3, parent )
	self.set_se3( se3 )




func activate( _root_call: bool = true ):
	body_state = BodyState.DYNAMIC

	update_physics_from_state()
	if _physical != null:
		_physical.sleeping = false
	







func deactivate( _root_call: bool = true ):
	body_state = BodyState.KINEMATIC
	
	remove_physical()


