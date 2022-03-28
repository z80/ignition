extends Control

var _target_obj = null
var _parent_gui = null

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui
	
	if not _target_obj.has_method( "get_coupled_child_node" ):
		return

	var s: Control = get_node( "RotateContainer/Angle" )
	var label: Label = get_node( "RotateContainer/Label" )
	
	var n: CouplingAttachment = _target_obj.get_coupled_child_node()
	if n != null:
		var angle: float = n.pitch * 180.0 / PI
		s.value = angle
		label.text = "rotation: " + str( angle )

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Delete_pressed():
	if _target_obj == null:
		return
	var construction_super_body = _target_obj.root_most_body()
	var construction = construction_super_body.construction
	construction.delete_block( _target_obj )
	_parent_gui.queue_free()


func _on_Angle_value_changed(value):
	var angle: float = value
	var n: CouplingAttachment = _target_obj.get_coupled_child_node()
	var label: Label = get_node( "RotateContainer/Label" )
	if n != null:
		n.pitch = angle / 180.0 * PI
		label.text = "rotation: " + str( angle )




