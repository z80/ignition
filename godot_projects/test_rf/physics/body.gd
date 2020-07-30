
extends Spatial
class_name Body

var _physical  = null  
var _state_local: RefFrame = null
var _state_root: RefFrame = null

func _ready():
	_state_local = RefFrame.new()
	_state_root  = RefFrame.new()


func _process( _delta ):
	var t: Transform = _state_root.t()
	transform = t


func _physics_process( delta ):
	if _physical:
		var t: Transform = _physical.transform
		var v: Vector3   = _physical.linear_velocity
		var w: Vector3   = _physical.angular_velocity
		_state_local.set_t( t )
		_state_local.set_v( v )
		_state_local.set_w( w )


# Should be overriden by inheriting cripts.
func create_physical():
	return null


func remove_physical():
	if _physical != null:
		_physical.queue_free()
		_physical = null

