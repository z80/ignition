extends Control

var _target_obj = null
var _parent_gui = null

var PartControlGroups = preload( "res://physics/parts/part_control_groups.gd" )

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui
	_init_menu()


# Called when the node enters the scene tree for the first time.
func _ready():
	if _target_obj != null:
		var group: int = _target_obj.control_group
		if group < PartControlGroups.ControlGroup.NONE:
			group = PartControlGroups.ControlGroup.NONE
		elif group > PartControlGroups.ControlGroup._5:
			group = PartControlGroups.ControlGroup._5
		_id_pressed( group )
	else:
		_id_pressed( PartControlGroups.ControlGroup.NONE )






func _init_menu():
	var menu_button = get_node("MenuButton")
	var menu: PopupMenu = menu_button.get_popup()
	menu.connect( "id_pressed", self, "_id_pressed" )
	menu.clear()
	for group in PartControlGroups.CONTROL_GROUP_IDS:
		var stri: String = group[0]
		var id: int = group[1]
		menu.add_item( stri, id, 0 )


func _id_pressed( id ):
	if _target_obj != null:
		_target_obj.control_group = id
	for group in PartControlGroups.CONTROL_GROUP_IDS:
		var group_id: int = group[1]
		if group_id == id:
			var menu_button: MenuButton = get_node("MenuButton")
			menu_button.text = group[0]
			


