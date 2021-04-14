
extends Panel

const Time = preload( "res://physics/utils/time.gd" )


var _cm: CelestialMotionRef = null
var _do_show: bool = false
var _target_name: String = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	_cm = CelestialMotionRef.new()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Timer_timeout():
	_update_celestial_motion()
	_visualize()


func _update_celestial_motion():
	var ctrl: Body = PhysicsManager.player_control as Body
	if ctrl == null:
		_do_show = false
		return
	var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )
	var p: Node = self.get_parent()
	var cb: CelestialSurface = ClosestCelestialBody.closest_celestial_body( p )
	if cb == null:
		_do_show = false
	var se3: Se3Ref = ctrl.relative_to( cb )
	_cm.init( cb.gm, se3 )
	_target_name = cb.name
	_do_show = true
	

func _visualize():
	if _do_show:
		_visualize_celestial_motion()
	else:
		_visualize_none()


func _visualize_celestial_motion():
	var l: Label = get_node( "Target" )
	l.text = _target_name
	l = get_node( "TrajectoryType" )
	var movement_type: String = _cm.movement_type()
	l.text = movement_type
	l = get_node( "Apogee" )
	l.text = str( _cm.apogee() * 0.001 )
	l = get_node( "Perigee" )
	l.text = str( _cm.perigee() * 0.001 )
	l = get_node( "TimeToPerigee" )
	var t: float = -_cm.time_after_periapsis()
	var stri: String = Time.seconds_to_str( t )
	l.text = stri
	l = get_node( "TimeToApogee" )
	if movement_type == "elliptic":
		var T: float = _cm.period()
		t = _cm.time_after_periapsis()
		t = T*0.5 - t
		stri = Time.seconds_to_str( t )
	else:
		l.text = "--:--"
	var v: float = _cm.max_velocity()
	l = get_node( "VPerigee" )
	l.text = "--:--"
	l = get_node( "VPerigee" )
	l.text = "--:--"
	l = get_node( "Period" )
	l.text = "--:--"
	l = get_node( "Velocity" )
	l.text = "--:--"


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
	l = get_node( "VPerigee" )
	l.text = "--:--"
	l = get_node( "Period" )
	l.text = "--:--"
	l = get_node( "Velocity" )
	l.text = "--:--"




