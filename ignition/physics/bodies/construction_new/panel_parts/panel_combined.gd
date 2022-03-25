extends Control

signal block_picked( block_desc )
signal launch
signal abort






# Called when the node enters the scene tree for the first time.
func _ready():
	pass
	



# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass







func _on_PanelCategories_category_picked( category: Resource ):
	var panel_blocks: Panel = get_node( "VBox/HBox/PanelBlocks" )
	panel_blocks.fill_blocks( category )



func _on_PanelBlocks_block_picked( block_desc: Resource ):
	emit_signal( "block_picked", block_desc )



func _on_PanelCommands_launch():
	emit_signal( "launch" )



func _on_Control_abort():
	emit_signal( "abort" )






