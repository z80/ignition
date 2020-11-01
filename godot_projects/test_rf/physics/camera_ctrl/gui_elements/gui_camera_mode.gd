extends Control

var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_fps_pressed():
	var camera = PhysicsManager.camera
	camera.set_mode_fps()
	
	_parent_gui.queue_free()


func _on_tps_pressed():
	var camera = PhysicsManager.camera
	camera.set_mode_fps()
	
	_parent_gui.queue_free()
