
extends Spatial

const LEN: float = 1.0+0.03
const SPEED_EPS: float = 0.001

var ball_yaw: float   = 0.0
var ball_pitch: float = 0.0
var ball_roll: float  = 0.0

var orientation: Quat = Quat.IDENTITY

var speed: float = 0.0

var LocalRefFrame = preload( "res://physics/utils/rotation_to.gd" )


func set_orientation_surface( se3_rel: Se3Ref ):
	# First determine lattitude of the opposite point.
	# We need it to make north direction to point towards north when rotated to local 
	# position of the object.
	var r: Vector3 = se3_rel.r
	var x: float = -r.x
	var y: float =  r.z
	var angle: float = atan2( y, x )
	var q_lattitude: Quat = Quat( Vector3.UP, angle )
	var local_up: Vector3 = se3_rel.r.normalized()
	var q_longtitude: Quat = LocalRefFrame.rotation_to( Vector3.UP, local_up )
	# Total transform
	var q_total: Quat = q_longtitude * q_lattitude
	# Navball relative to the object.
	# Object relative to navball:
	#var q_debug: Quat = q_total.inverse() * se3_rel.q
	var q_rel: Quat = se3_rel.q.inverse() * q_total
	orientation = q_rel
	
	DDD.important()
	DDD.print( "set_orientation_surface( " + str(orientation) + " )", 5.0, "set_orientation_surface" )


func set_orientation_orbit( se3_rel: Se3Ref ):
	# Here it is simple.
	# Navball represents the planet.
	# We have object relative to the planet.
	# And we want the opposite: planet relative to the object.
	orientation = se3_rel.q.inverse()

	DDD.important()
	DDD.print( "set_orientation_orbit( " + str(orientation) + " )", 5.0, "set_orientation_orbit" )





func set_prograde( v: Vector3, visible: bool = true ):
	_set_indicator( "Prograde", v, visible )



func set_retrograde( v: Vector3, visible: bool = true ):
	_set_indicator( "Retrograde", v, visible )



func set_radial_out( v: Vector3, visible: bool = true ):
	_set_indicator( "RadialOut", v, visible )



func set_radial_in( v: Vector3, visible: bool = true ):
	_set_indicator( "RadialIn", v, visible )



func set_normal( v: Vector3, visible: bool = true ):
	_set_indicator( "Normal", v, visible )



func set_anti_normal( v: Vector3, visible: bool = true ):
	_set_indicator( "AntiNormal", v, visible )



func _process(delta):
	_update_orientation()



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.



func _update_orientation():
#	var q_yaw: Quat = Quat( Vector3.UP, ball_yaw )
#	var q_pitch: Quat = Quat( Vector3.RIGHT, ball_pitch )
#	var q_roll: Quat = Quat( Vector3.BACK, ball_roll )
#	var q: Quat = q_yaw * q_pitch * q_roll
	var n: Spatial = get_node( "navball" )
	n.transform.basis = Basis( orientation )
	
	var read_back: Quat = Quat(n.transform.basis)
	
	DDD.print( "update_orientation: " + str(orientation) + ", read_back: " + str(read_back) )



# All except ones not having a pair one.
func _set_indicator( name: String, v: Vector3, visible: bool = true ):
	var s: Spatial = get_node( name )
	if s == null:
		return
	if not visible:
		s.visible = false
		return
	
	var t: Transform = s.transform
	var l: float = v.length()
	if l < SPEED_EPS:
		s.visible = false
		return
	
	var r: Vector3 = v.normalized()
	if r.y < -0.1:
		s.visible = false
		return
	s.visible = true
	r.y = LEN
	t.origin = r
	
	t.basis = Quat.IDENTITY
	
	s.transform = t







