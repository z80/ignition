
extends MenuBase


var SaveLoad = preload( "res://physics/singletons/save_load.gd" )

var _menu_stack: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	var default_name: String = _default_file_name()
	var s: LineEdit = _get_file_name_node()
	s.text = default_name
	
	_fill_files_node()



func _fill_files_node():
	var files: Array = SaveLoad.saved_files()
	var list: ItemList = _get_file_list_node()
	for name in files:
		list.add_item( name, null, true )
	
	list.select( 0 )





func _get_file_name_node():
	var n: LineEdit = get_node( "FileName" )
	return n


func _get_file_list_node():
	var l: ItemList = get_node( "Scroll/List" )
	return l


func _default_file_name():
	var d: Dictionary = OS.get_datetime()
	var name: String = "{year}_{month}_{day}_{hour}_{minute}_{second}".format( d )
	return name



func _on_Save_pressed():
	UiSound.play( Constants.ButtonClick )
	var file_node: LineEdit = _get_file_name_node()
	var file_name: String = file_node.text
	var full_name: String = SaveLoad.full_file_path( file_name )
	
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	var data: Dictionary = S.serialize_all( root )
	#var stri: String = JSON.print( data )
	var file = File.new()
	file.open( full_name, File.WRITE )
	file.store_var( data, true )
	file.close()
	
	self.pop()


func _on_Cancel_pressed():
	UiSound.play( Constants.ButtonClick )
	self.pop()





