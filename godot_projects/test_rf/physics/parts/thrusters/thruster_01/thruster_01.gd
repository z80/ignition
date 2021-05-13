
extends Thruster

#export(float)  var throttle = 100.0 setget set_throttle, get_throttle

var user_input: Dictionary

func init():
	var Visual   = load( "res://physics/parts/thrusters/thruster_01/thruster_01_visual.tscn")
	var Physical = load( "res://physics/parts/thrusters/thruster_01/thruster_01_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()







func set_collision_layer( layer ):
	if _physical:
		var rb = _physical
		rb.set_collision_layer( layer )



func create_physical():
	var ph = .create_physical()



func activate( root_part: bool = true ):
	.activate( root_part )
	if _physical != null:
		_physical.mode = RigidBody.MODE_RIGID
		_physical.sleeping = false


func deactivate( root_part: bool = true ):
	.deactivate( root_part )
	if _physical != null:
		_physical.mode = RigidBody.MODE_KINEMATIC



func set_enabled( en ):
	if _physical != null:
		_physical.on = en
	if _visual != null:
		_visual.enabled = en


func set_throttle( th: float ):
	if th < 0.0:
		th = 0.0
	elif th > 1.0:
		th = 1.0
	if _physical != null:
		_physical.throttle = th
	if _visual != null:
		_visual.throttle = th


func get_throttle():
	if _physical != null:
		var th: float = _physical.throttle
		return th
	
	return 1.0


# Overriding physics body removal.
# First disabling everything, after that 
# proceeding with default bahavior.
func remove_physical():
	set_enabled( false )
	.remove_physical()
