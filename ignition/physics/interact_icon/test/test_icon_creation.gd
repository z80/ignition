extends Node2D

var scene: PackedScene = preload( "res://physics/interact_icon/container_window.tscn" )


# Called when the node enters the scene tree for the first time.
func _ready():
	var s: Node = scene.instantiate()
	add_child( s )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
