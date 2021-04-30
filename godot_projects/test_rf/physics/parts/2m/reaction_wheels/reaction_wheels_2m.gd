
extends Part


func init():
	var Visual   = load( "res://physics/bodies/parts/2m/reaction_wheels/reaction_wheels_2m_visual.tscn")
	var Physical = load( "res://physics/bodies/parts/2m/reaction_wheels/reaction_wheels_2m_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()


func process_user_input_2( input: Dictionary ):
	if _physical:
		_physical.user_input = input.duplicate( true )
		if _physical.user_input.size() != 0:
			_physical.sleeping = false
			print( "wheels input: ", _physical.user_input )



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


