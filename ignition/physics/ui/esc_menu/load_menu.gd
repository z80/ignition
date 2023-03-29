
extends MenuBase


var SaveLoad = preload( "res://physics/singletons/save_load.gd" )

var _menu_stack: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	_fill_files_node()



func _fill_files_node():
	var files: Array = SaveLoad.saved_files()
	var list: ItemList = _get_file_list_node()
	for name in files:
		list.add_item( name, null, true )
	
	list.select( 0 )




func _get_file_list_node():
	var l: ItemList = get_node( "Scroll/List" )
	return l



func _get_selected_file_name():
	var l: ItemList = _get_file_list_node()
	var items: PoolIntArray = l.get_selected_items()
	if items.size() < 1:
		return ""
	var index: int = items[0]
	var file_name: String = l.get_item_text( index )
	var full_name: String = SaveLoad.full_file_path( file_name )
	return full_name


func _on_Load_pressed():
	UiSound.play( Constants.ButtonClick )
	
	var fname: String = _get_selected_file_name()
	
	var root: Node = RootScene.ref_frame_root
	
	var file = File.new()
	var err: int = file.open( fname, File.READ )
	if err != OK:
		return
	
	var data: Dictionary = file.get_var( true )
	file.close()
	
	SaveLoad.deserialize_all( root, data )
	
	self.pop()


func _on_Cancel_pressed():
	UiSound.play( Constants.ButtonClick )
	self.pop()


func on_user_input( _event: InputEvent ):
	pass




