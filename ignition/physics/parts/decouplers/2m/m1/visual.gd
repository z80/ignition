
extends RigidBody3D


func get_decoupling_node():
	var n: Node = get_node( "Model/Top" )
	return n


