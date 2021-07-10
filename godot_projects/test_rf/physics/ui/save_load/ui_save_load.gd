extends Control


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_Save_pressed():
	var S = load( "res://physics/singletons/save_load.gd" )
	var root: Node = BodyCreator.root_node
	var data: Dictionary = S.serialize( root )
	print( "data: \n", data )


func _on_Load_pressed():
	print( "Is\'s supposed to load a game here" )
	pass # Replace with function body.
