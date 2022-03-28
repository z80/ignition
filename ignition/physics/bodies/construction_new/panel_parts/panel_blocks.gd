
extends Control

signal block_picked( desc )

export(PackedScene) var button_scene = null

var _container: GridContainer = null


func fill_blocks( category: Resource ):
	_clear()
	_fill_blocks( category )



func on_block_picked( block_desc: Resource ):
	emit_signal( "block_picked", block_desc )



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.



func _fill_blocks( category: Resource ):
	var blocks: Array = Game.get_blocks( category )
	for p in blocks:
		_create_button(p)



func _clear():
	var c: GridContainer = _get_container()
	var qty: int = c.get_child_count()
	for i in range(qty):
		var ch: Node = c.get_child(i)
		ch.queue_free()



func _get_container():
	if _container == null:
		_container = get_node( "Scroll/Container" )
	
	return _container



func _create_button( block_desc: Resource ):
	var vb: GridContainer = _get_container()
	var btn: Control      = button_scene.instance()
	btn.block_desc        = block_desc
	vb.add_child( btn )
	btn.connect( "block_picked", self, "on_block_picked" )





