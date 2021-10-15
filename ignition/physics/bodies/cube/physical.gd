
extends Node

var transform: Transform setget set_transform, get_transform
var linear_velocity: Vector3 setget set_linear_velocity, get_linear_velocity
var angular_velocity: Vector3 setget set_angular_velocity, get_angular_velocity
var collision_layer: int setget set_collision_layer, get_collision_layer


func set_transform( t: Transform ):
	$RigidBody.transform = t
	
func get_transform():
	return $RigidBody.transform

func set_linear_velocity( v: Vector3 ):
	$RigidBody.linear_velocity = v


func get_linear_velocity():
	return $RigidBody.linear_velocity


func set_angular_velocity( w: Vector3 ):
	$RigidBody.angular_velocity = w


func get_angular_velocity():
	return $RigidBody.angular_velocity


func set_collision_layer( v: int ):
	$RigidBody.collision_layer = v
	$RigidBody.collision_mask  = v


func get_collision_layer():
	return $RigidBody.collision_layer
