extends HBoxContainer

@export var Grab: PackedScene = null

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
	_activate_grab( _target_obj )
	_parent_gui.queue_free()



func _activate_grab( body ):
	if not is_instance_valid( body ):
		return
	var grab = Grab.instantiate()
	RootScene.get_root_for_gui_popups().add_child( grab )
	grab.target = body
	
	body.set_meta( "grab", grab )


