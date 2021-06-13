extends Control

var _target_obj = null
var _parent_gui = null


func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _process( _delta: float ):
	_update_from_object()





func _update_from_object():
	var real_obj: bool = is_instance_valid( _target_obj )
	if not real_obj:
		return
	
	var left: float = _target_obj._fuel_left
	var full: float = _target_obj._fuel_initial
	
	var v: float = (left * 100.0) / full
	var indicator = get_node( "Container/Indicator" )
	indicator.value = v
	
	var label = get_node( "Container/Label" )
	label.text = "fuel left: " + str( left )
