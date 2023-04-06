extends Node2D

@export var container_window_scene: PackedScene = null

# Can be overwritten in order to show different container window.
func get_container_scene():
	return container_window_scene

# Called when the node enters the scene tree for the first time.
func _ready():
	var s: PackedScene = get_container_scene()
	var inst: Node = s.instantiate()
	add_child( inst )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
