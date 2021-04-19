extends Spatial

const LEN: float = 1.0+0.03
const SPEED_EPS: float = 0.001

var ball_yaw: float   = 0.0
var ball_pitch: float = 0.0
var ball_roll: float  = 0.0

var speed: float = 0.0

func set_orientation( yaw: float, pitch: float, roll: float ):
	ball_yaw   = -yaw
	ball_pitch = -pitch
	ball_roll  = -roll

func set_prograde( v: Vector3 ):
	_set_indicator( "Prograde", v )

func set_retrograde( v: Vector3 ):
	_set_indicator( "Retrograde", v )


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


func _set_indicator( name: String, v: Vector3 ):
	var s: Spatial = get_node( name )
	if s == null:
		return
	var t: Transform = s.transform
	var l: float = v.length()
	if l < SPEED_EPS:
		s.visible = false
		return
	
	s.visible = true
	var r: Vector3 = v.normalized()
	r *= LEN
	t.origin = r
	
	var alpha: float = atan2( r.x, r.z )
	var beta: float  = acos( r.y )
	var q_alpha: Quat = Quat( Vector3.UP, alpha )
	var q_beta: Quat  = Quat( Vector3.RIGHT, beta )
	var q: Quat = q_alpha * q_beta
	t.basis = q
	
	s.transform = t







