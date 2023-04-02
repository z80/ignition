extends Control

var _target_obj = null
var _parent_gui = null
var _index_to_group_id: Array = []

var PartControlGroups = preload( "res://physics/parts/part_control_groups.gd" )

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui
	_init_menu()
	#_init_group()


# Called when the node enters the scene tree for the first time.
func _init_group():
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
	var _err_code: int = menu.connect("id_pressed", Callable(self, "_id_pressed"))
	menu.clear()
	var index: int = 0
	_index_to_group_id.clear()
	for group in PartControlGroups.CONTROL_GROUP_IDS:
		var stri: String = group[0]
		var id: int = index
		_index_to_group_id.push_back( [index, group[1]] )
		index += 1
		menu.add_item( stri, id, 0 )
	_init_from_object()


func _id_pressed( index ):
	var group: int = PartControlGroups.ControlGroup.NONE
	for g in _index_to_group_id:
		var ind: int = g[0]
		if ind == index:
			group = g[1]
			break
	if _target_obj != null:
		_target_obj.control_group = group
	for g in PartControlGroups.CONTROL_GROUP_IDS:
		var group_id: int = g[1]
		if group_id == group:
			var menu_button: MenuButton = get_node("MenuButton")
			menu_button.text = g[0]
			break
			


func _init_from_object():
	if _target_obj == null:
		return
	var control_group: int = _target_obj.control_group
	for group in PartControlGroups.CONTROL_GROUP_IDS:
		var id: int = group[1]
		if id == control_group:
			var menu_button: MenuButton = get_node("MenuButton")
			menu_button.text = group[0]






