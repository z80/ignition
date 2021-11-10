extends Control


signal world
signal local

var world_btn: Button = null
var local_btn: Button = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var qty: int = get_child_count()
	for i in range(qty):
		var ch: Node = get_child(i)
		var name: String = ch.name
		var path: String = ch.get_path()
		print( name, ": ", path )
	world_btn = get_node( "World" )
	local_btn = get_node( "Local" )


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_World_pressed():
	world_btn.pressed = true
	local_btn.pressed = false
	emit_signal( "world" )


func _on_Local_pressed():
	world_btn.pressed = false
	local_btn.pressed = true
	emit_signal( "local" )
