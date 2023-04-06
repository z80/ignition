extends Node2D

@export var scene: PackedScene = null

# Called when the node enters the scene tree for the first time.
func _ready():
	var s: Node = scene.instantiate()
	add_child( s )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
