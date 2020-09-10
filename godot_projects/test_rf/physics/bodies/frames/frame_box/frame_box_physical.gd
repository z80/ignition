
extends Node

var collision_layer: int setget set_collision_layer, get_collision_layer



func set_collision_layer( v: int ):
	$RigidBody.collision_layer = v
	$RigidBody.collision_mask  = v


func get_collision_layer():
	return $RigidBody.collision_layer
