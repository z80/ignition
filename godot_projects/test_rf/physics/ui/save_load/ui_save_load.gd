extends Control


var _data: Dictionary = {}


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


func _on_Save_pressed():
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	var data: Dictionary = S.serialize_all( root )
	_data = data
	print( "data: \n", data )


func _on_Load_pressed():
	print( "Is\'s supposed to load a game here" )
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	S.deserialize_all( root, _data )
