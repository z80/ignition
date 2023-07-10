extends Control

var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_FinishEditing_pressed():
	if _target_obj == null:
		return
	var s = _target_obj.root_most_body()
	var grab = s.get_meta( "grab" )
	s.set_meta( "grab", null )
	if (grab != null) and (is_instance_valid(grab)):
		grab.queue_free()
	
	_parent_gui.queue_free()
