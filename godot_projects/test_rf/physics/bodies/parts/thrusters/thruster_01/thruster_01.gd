
extends Body


var user_input: Dictionary

func init():
	var Visual   = load( "res://physics/bodies/parts/thrusters/thruster_01/thruster_01_visual.tscn")
	var Physical = load( "res://physics/bodies/parts/thrusters/thruster_01/thruster_01_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()


func process_user_input_2( input: Dictionary ):
	user_input = input.duplicate( true )
	if _physical:
		if user_input.has( "ui_space" ):
			var inp = user_input[ "ui_space" ]
			if inp.pressed:
				var en: bool = _physical.enabled
				en = not en
				set_enabled( en )
		
		if user_input.has( "ui_z" ):
			var inp = user_input[ "ui.z" ]
			if inp.pressed:
				var th: float = _physical.throttle
				th -= 0.1*th
				set_throttle( th )
		
		if user_input.has( "ui_x" ):
			var inp = user_input[ "ui.x" ]
			if inp.pressed:
				var th: float = _physical.throttle
				th += 0.1*th
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
		_physical.enabled = en
	if _visual != null:
		_visual.enabled = en


func set_throttle( th: float ):
	if _physical:
		_physical.throttle = th
	if _visual:
		_visual.throttle = th


# Overriding physics body removal.
# First disabling everything, after that 
# proceeding with default bahavior.
func remove_physical():
	set_enabled( false )
	.remove_physical()
