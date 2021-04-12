
extends Panel


var _cm: CelestialMotionRef = null
var _do_show: bool = false


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
	var ctrl = PhysicsManager.player_control as Body
	if ctrl == null:
		_do_show = false
	_do_show = true
	

func _visualize():
	if _do_show:
		_visualize_celestial_motion()
	else:
		_visualize_none()


func _visualize_celestial_motion():
	pass


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




