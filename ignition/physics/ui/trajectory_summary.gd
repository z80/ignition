
extends Panel

const TimeUtils: Resource = preload( "res://physics/utils/time.gd" )


var _cm: CelestialMotionRef = null
var velocity: float = 0.0
var distance: float = 0.0
var _do_show: bool = false
var _target_name: String = ""
var _labels: Dictionary = {}

# Called when the node enters the scene tree for the first time.
func _ready():
	_cm = CelestialMotionRef.new()
	_init_labels()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Timer_timeout():
	_update_celestial_motion()
	_visualize()


func _update_celestial_motion():
#	return
	
	var ctrl: PhysicsBodyBase = RootScene.ref_frame_root.player_camera.get_parent() as PhysicsBodyBase
	if ctrl == null:
		_do_show = false
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = ctrl.get_parent()
	var cb: CelestialBody = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		_do_show = false
		return
	var se3: Se3Ref = ctrl.relative_to( cb )
	_cm.allow_orbiting = true
	var _ok: bool = _cm.launch( cb.own_gm, se3 )
	velocity = se3.v.length()
	distance = se3.r.length()
	_target_name = cb.name
	_do_show = true
	
	var h: float = _cm.specific_angular_momentum()
	print( "h: ", h, " type: ", _cm.movement_type() )
	

func _visualize():
	if _do_show:
		_visualize_celestial_motion()
	else:
		_visualize_none()


func _visualize_celestial_motion():
	var l: Label = _labels[ "target" ]
	l.text = _target_name
	l = _labels[ "trajectory_type" ]
	var movement_type: String = _cm.movement_type()
	l.text = movement_type
	l = _labels[ "apogee" ]
	l.text = str( _cm.apogee() * 0.001 ) + "km"
	l = _labels[ "perigee" ]
	l.text = str( _cm.perigee() * 0.001 ) + "km"
	l = _labels[ "time_to_perigee" ]
	var t: float = abs( _cm.time_after_periapsis() )
	var stri: String = TimeUtils.seconds_to_str( t )
	l.text = stri
	l = _labels[ "time_to_apogee" ]
	if movement_type == "elliptic":
		var T: float = _cm.period()
		t = abs( _cm.time_after_periapsis() )
		t = T*0.5 - t
		stri = TimeUtils.seconds_to_str( t )
		l.text = stri
	else:
		l.text = "--:--"
	var v: float = _cm.max_velocity()
	l = _labels[ "v_perigee" ]
	l.text = str(v) + "m/s"
	
	if movement_type == "elliptic":
		v = _cm.min_velocity()
	elif (movement_type == "hyperbolic") or (movement_type == "parabolic"):
		v = _cm.excess_velocity()
	l = _labels[ "v_apogee" ]
	l.text = str( v ) + "m/s"

	l = _labels[ "period" ]
	if movement_type == "elliptic":
		var T: float = _cm.period()
		stri = TimeUtils.seconds_to_str( T )
		l.text = stri
	else:
		l.text = "--:--"
	#v = _cm.get
	l = _labels[ "velocity" ]
	l.text = str( velocity ) + "m/s"
	l = _labels[ "distance" ]
	l.text = str( distance * 0.001 ) + "km"


func _visualize_none():
	var l: Label = get_node( "Target" )
	l.text = "--:--"
	l = get_node( "TrajectoryType" )
	l.text = "--:--"
	l = get_node( "Apogee" )
	l.text = "--:--"
	l = get_node( "Perigee" )
	l.text = "--:--"
	l = get_node( "TimeToPerigee" )
	l.text = "--:--"
	l = get_node( "TimeToApogee" )
	l.text = "--:--"
	
	l = get_node( "VPerigee" )
	l.text = "--:--"
	l = get_node( "VApogee" )
	l.text = "--:--"
	l = get_node( "Period" )
	l.text = "--:--"
	l = get_node( "Velocity" )
	l.text = "--:--"



func _init_labels():
	var l: Label = get_node( "Target" )
	_labels["target"] = l
	l = get_node( "TrajectoryType" )
	_labels["trajectory_type"] = l
	l = get_node( "Apogee" )
	_labels["apogee"] = l
	l = get_node( "Perigee" )
	_labels["perigee"] = l
	l = get_node( "TimeToPerigee" )
	_labels["time_to_perigee"] = l
	l = get_node( "TimeToApogee" )
	_labels["time_to_apogee"] = l
	l = get_node( "VPerigee" )
	_labels["v_perigee"] = l
	l = get_node( "VApogee" )
	_labels["v_apogee"] = l
	l = get_node( "Period" )
	_labels["period"] = l
	l = get_node( "Velocity" )
	_labels["velocity"] = l
	l = get_node( "Distance" )
	_labels["distance"] = l

