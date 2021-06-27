extends Node


func get_root_for_bodies():
	var ret = get_node( "bodies" )
	return ret


func get_root_for_physics_envs():
	var ret = get_node( "physics_envs" )
	return ret


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
