
extends Spatial
class_name Body

var _visual    = null
var _physical  = null  
var _state_local: RefFrame = null
var _state_root: RefFrame = null

func _ready():
	_state_local = RefFrame.new()
	_state_root  = RefFrame.new()


func _process( _delta ):
	pass

func _physics_process( delta ):
	if _physical:
		var t: Transform = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		_state_local.set_t( t )
		_state_local.set_v( v )
		_state_local.set_w( w )


func create_visual():
	return null


func remove_visual():
	pass


func create_physical():
	return null


func remove_physical():
	pass

