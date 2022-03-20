
extends Panel

signal category_picked( category )

export(PackedScene) var button_scene = null

var _vbox: VBoxContainer = null


# Called when the node enters the scene tree for the first time.
func _ready():
	_fill_categories()


func _get_container():
	if _vbox == null:
		_vbox = get_node( "Scroll/Container" )
	
	return _vbox


func _clear():
	var vb: VBoxContainer = _get_container()
	var qty: int = vb.get_child_count()
	for i in range(qty):
		var n: Node = vb.get_child( i )
		n.queue_free()



func _fill_categories():
	var cats: Array = Game.get_categories()
	for cat in cats:
		_create_button( cat )
	


func _create_button( category: Resource ):
	var vb: VBoxContainer = _get_container()
	var btn: Control      = button_scene.instance()
	btn.category = category
	vb.add_child( btn )
	btn.connect( "category_picked", self, "on_category_picked" )
	
	

func on_category_picked( category: Resource ):
	emit_signal( "category_picked", category )



