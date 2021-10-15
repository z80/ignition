extends Control

var _target_obj = null
var _parent_gui = null


func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _process( _delta ):
	if (_target_obj == null) or (not is_instance_valid(_target_obj)):
		self.queue_free()
		return
	
	var pb: ProgressBar = get_node( "CenterContainer/ProgressBar" )
	pb.value = _target_obj.throttle * 100.0
	#pass

func _on_ControlThis_pressed():
	if not is_instance_valid( _target_obj ):
		return
	
	PhysicsManager.player_control = _target_obj
	PhysicsManager.camera.apply_target()
	
	_parent_gui.queue_free()
