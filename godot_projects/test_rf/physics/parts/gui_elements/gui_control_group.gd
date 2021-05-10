extends Control

var _target_obj = null
var _parent_gui = null

const GROUPS: Array = [
	["Group none", -1], 
	["Group any",   0], 
	["Group 1", 1], 
	["Group 2", 2], 
	["Group 3", 3], 
	["Group 4", 4], 
	["Group 5", 5]
]

func init( target_obj, parent_gui ):
	_target_obj = target_obj
	_parent_gui = parent_gui
	_init_menu()


# Called when the node enters the scene tree for the first time.
func _ready():
	if _target_obj != null:
		var group: int = _target_obj.control_group
		if group < -1:
			group = -1
		elif group > 5:
			group = 5
		_id_pressed( group )
	else:
		_id_pressed( -1 )




func _on_menu_pressed():
	pass # Replace with function body.




func _init_menu():
	var menu_button = get_node("MenuButton")
	var menu: PopupMenu = menu_button.get_popup()
	menu.connect( "id_pressed", self, "_id_pressed" )
	menu.clear()
	for group in GROUPS:
		var stri: String = group[0]
		var id: int = group[1]
		menu.add_item( stri, id, 0 )


func _id_pressed( id ):
	if _target_obj != null:
		_target_obj.control_group = id
	for group in GROUPS:
		var group_id: int = group[1]
		if group_id == id:
			var menu_button: MenuButton = get_node("MenuButton")
			menu_button.text = group[0]
			


