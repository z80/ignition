
extends Part
class_name Character

export(float) var speed     = 3.0
export(float) var gain      = 50.5
export(float) var max_force = 5.0


export(float) var ang_vel        = 1.0
export(float) var max_ang_vel    = 2.0
export(float) var gain_angular   = 100.0
export(float) var gain_d_angular = 100.0
export(float) var max_torque     = 5.0

export(bool) var translation_abolute = false
export(bool) var rotation_abolute    = false

# Amount of force projected onto this direction is zeroed.
export(bool) var translation_do_ignore_direction = true
export(Vector3) var translation_ignore_direction = Vector3.UP

var target_q: Quat    = Quat.IDENTITY
var local_up: Vector3 = Vector3.UP

var print_period: float  = 0.1
var print_elapsed: float = 0.0

var _input: Dictionary = {}




func init():
	.init()


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


# This one should be overriden by implementations.
func process_user_input_group( input: Dictionary ):
	pass


func process_inner( delta ):
	.process_inner( delta )
	# Update visual animation state.
	if (_physical != null) and (_visual != null):
		# This is for visualizing walk.
		var s: float = _physical.get_speed_normalized()
		_visual.set_speed_normalized( s )



func privot_tps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_tps
	return null


func privot_fps( ind: int = 0 ):
	if is_instance_valid( _visual ):
		return _visual.target_fps
	return null



func set_local_up( up: Vector3 ):
	if _physical != null:
		_physical.local_up = up


