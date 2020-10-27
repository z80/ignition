
extends Body

func _ready():
	init()


func init():
	var Visual   = load( "res://physics/bodies/character_simple/character_simple_visual.tscn")
	var Physical = load( "res://physics/bodies/character_simple/character_simple_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
	
	
	var t: Transform
	t.origin = Vector3( 0.0, 20.0, 0.0 )
	set_t( t )


func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.set_collision_layer( layer )


func process_inner( delta ):
	.process_inner( delta )
	# Update visual animation state.
	var s: float = _physical.get_speed_normalized()
	_visual.set_speed_normalized( s )


func process_user_input( _event: InputEvent ):
	if not is_instance_valid( _physical ):
		return
	
	_physical.apply_user_input = true







func privot_tps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_tps
	return null

func privot_fps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_fps
	return null
