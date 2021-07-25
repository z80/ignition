extends Control


#var _data: Dictionary = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Save_pressed():
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	var data: Dictionary = S.serialize_all( root )
	#var stri: String = JSON.print( data )
	var file = File.new()
	file.open( "res://save_game.json", File.WRITE )
	file.store_var( data, true )
	file.close()

	#_data = data
	print( "data: \n", data )


func _on_Load_pressed():
	print( "Is\'s supposed to load a game here" )
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	
	var file = File.new()
	file.open( "res://save_game.json", File.READ )
	var data: Dictionary = file.get_var( true )
	file.close()
	
	S.deserialize_all( root, data )
