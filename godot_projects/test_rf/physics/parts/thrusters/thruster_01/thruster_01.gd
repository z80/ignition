
extends Part

export(float)  var throttle = 100.0 setget set_throttle, get_throttle

var user_input: Dictionary

func init():
	var Visual   = load( "res://physics/parts/thrusters/thruster_01/thruster_01_visual.tscn")
	var Physical = load( "res://physics/parts/thrusters/thruster_01/thruster_01_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()


func process_user_input_2( input: Dictionary ):
	user_input = input.duplicate( true )
	if _physical:
		if user_input.has( "ui_space" ):
			var inp = user_input[ "ui_space" ]
			if inp.pressed:
				var en: bool = _physical.on
				en = not en
				set_enabled( en )
		
		if user_input.has( "ui_z" ):
			var inp = user_input[ "ui_z" ]
			if inp.pressed:
				var th: float = _physical.throttle
				th -= 0.1
				set_throttle( th )
		
		if user_input.has( "ui_x" ):
			var inp = user_input[ "ui_x" ]
			if inp.pressed:
				var th: float = _physical.throttle
				th += 0.1
				set_throttle( th )


func gui_classes( mode: String = "" ):
	var classes = []
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	if mode.length() < 1:
		var Status = load( "res://physics/bodies/parts/thrusters/thruster_01/gui_thruster_status.tscn" )
		classes.push_back( Status )
	
	return classes


func set_collision_layer( layer ):
	if _physical:
		var rb = _physical
		rb.set_collision_layer( layer )



func create_physical():
	var ph = .create_physical()



func activate():
	.activate()
	if _physical != null:
		_physical.mode = RigidBody.MODE_RIGID
		_physical.sleeping = false


func deactivate():
	.deactivate()
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
