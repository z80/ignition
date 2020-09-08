extends HBoxContainer


var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Grab_pressed():
	var s = _target_obj.root_most_body()
	s.activate_grab( _target_obj )


func _on_Rotate_pressed():
	var s = _target_obj.root_most_body()
	s.activate_rotate( _target_obj )
