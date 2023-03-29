extends Control

var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_ControlThis_pressed():
	if not is_instance_valid( _target_obj ):
		return
	
	RootScene.ref_frame_root.player_control = _target_obj
	RootScene.ref_frame_root.player_camera.apply_target()
	
	_parent_gui.queue_free()
