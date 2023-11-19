extends Control

var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Delete_pressed():
	if _target_obj == null:
		return
	var construction_assembly = _target_obj.get_ref_frame_root_most_body()
	var construction = construction_assembly.construction
	construction.delete_block( _target_obj )
	_parent_gui.queue_free()
