extends Node

var Rf = preload( "res://rf.tscn" )
var _rf_tree = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_rf_tree = RefFrameTree.new()
	pass # Replace with function body.


func ref_frame():
	var rf = Rf.instance()
	_rf_tree.push_back( rf )
	return rf

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
