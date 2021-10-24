extends Control


signal world
signal local

var world_btn: Button = null
var local_btn: Button = null


# Called when the node enters the scene tree for the first time.
func _ready():
	var world_btn: Button = get_node( "World" )
	var local_btn: Button = get_node( "Local" )


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
