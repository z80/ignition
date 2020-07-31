
extends Node


# create instance
func create( type_name: String ):
	match type_name:
		"box":
			return box()
		"plane":
			return plane()




func _create( CL ):
	var inst = CL.instance()
	get_tree().get_root().add_child( inst )
	return inst



func box():
	var B = preload( "res://physics/bodies/cube/cube.tscn" )
	var b = _create( B )
	return b


func plane():
	var B = preload( "res://physics/bodies/plane/plane.tscn" )
	var b = _create( B )
	return b


