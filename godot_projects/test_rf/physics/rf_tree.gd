
extends Node

var _rf_tree = null

# Called when the node enters the scene tree for the first time.
func _ready():
	_rf_tree = RefFrameTree.new()
	pass # Replace with function body.


func add( rf ):
	if ( rf._ref_frame != null ):
		return
	rf._ref_frame = RefFrame.new()
	_rf_tree.push_back( rf._ref_frame )
	return rf


func remove( rf ):
	_rf_tree.remove( rf._ref_frame )
	rf._ref_frame = null

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
