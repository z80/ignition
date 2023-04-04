extends Control


#var _data: Dictionary = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Save_pressed():
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = RootScene.ref_frame_root
	var data: Dictionary = S.serialize_all( root )
	#var stri: String = JSON.print( data )
	var file: FileAccess = FileAccess.open( "res://save_game.json", FileAccess.WRITE )
	assert( file != null, "Failed to open file for writing!" )
	file.store_var( data, true )
	file.close()

	#_data = data
	print( "data: \n", data )


func _on_Load_pressed():
	print( "Is\'s supposed to load a game here" )
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = RootScene.ref_frame_root
	
	var file: FileAccess = FileAccess.open( "res://save_game.json", FileAccess.READ )
	assert( file != null, "Failed to open file for reading!" )
	var data: Dictionary = file.get_var( true )
	file.close()
	
	S.deserialize_all( root, data )
