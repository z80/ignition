extends Spatial


var ball_yaw: float   = 0.0
var ball_pitch: float = 0.0
var ball_roll: float  = 0.0

var speed: float = 0.0

func set_orientation( yaw: float, pitch: float, roll: float ):
	ball_yaw   = -yaw
	ball_pitch = -pitch
	ball_roll  = -roll


func _process(delta):
	_update_orientation()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _update_orientation():
	var q_yaw: Quat = Quat( Vector3.UP, ball_yaw )
	var q_pitch: Quat = Quat( Vector3.RIGHT, ball_pitch )
	var q_roll: Quat = Quat( Vector3.BACK, ball_roll )
	var q: Quat = q_yaw * q_pitch * q_roll
	var n: Spatial = get_node( "navball" )
	n.transform.basis = q










