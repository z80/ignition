extends Control

var _target_obj = null

func init( target_obj ):
	_target_obj = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Enter_pressed():
	if _target_obj == null:
		return
	var has_enter = _target_obj.has_method( "enter" )
	if has_enter:
		_target_obj.call_method( "enter" )
