extends Control


var _root_ui: Control = null


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _get_root_ui():
	if _root_ui == null:
		_root_ui = get_parent()
	return _root_ui 


func _on_TrajParams_pressed():
	UiSound.play( Constants.ButtonClick )

	var b: Button = get_node( "TrajParams" )
	var v: bool = b.button_pressed
	
	var r: Node = _get_root_ui()
	var tp: Node = r.get_node( "TrajParams" )
	tp.visible = v


func _on_CtrlGroups_pressed():
	UiSound.play( Constants.ButtonClick )

	var b: Button = get_node( "CtrlGroups" )
	var v: bool = b.button_pressed
	
	var r: Node = _get_root_ui()
	var tp: Node = r.get_node( "CtrlGroups" )
	tp.visible = v


func _on_NavBall_pressed():
	UiSound.play( Constants.ButtonClick )

	var b: Button = get_node( "NavBall" )
	var v: bool = b.button_pressed
	
	var r: Node = _get_root_ui()
	var tp: Node = r.get_node( "NavBall" )
	tp.visible = v






