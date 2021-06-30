
extends Part


func init():
	var Visual   = load( "res://physics/parts/reaction_control_wheels/2m/m1/visual.tscn")
	var Physical = load( "res://physics/parts/reaction_control_wheels/2m/m1/physical.tscn" )
	
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



func activate( root_part: bool = true ):
	.activate( root_part )
	if _physical != null:
		_physical.sleeping = false


func deactivate( root_part: bool = true ):
	.deactivate( root_part )


