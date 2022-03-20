extends Control

signal block_picked( block_desc )


func on_category_picked( category: Resource ):
	var panel_blocks: Panel = get_node( "HBox/PanelBlocks" )
	panel_blocks.fill_blocks( category )


func on_block_picked( block_desc: Resource ):
	emit_signal( "block_picked", block_desc )


# Called when the node enters the scene tree for the first time.
func _ready():
	var panel_categories: Panel = get_node( "HBox/PanelCategories" )
	panel_categories.connect( "category_picked", self, "on_category_picked" )

	var panel_blocks: Panel = get_node( "HBox/PanelBlocks" )
	panel_blocks.connect( "block_picked", self, "on_block_picked" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass





