
extends Node


# It is where all visual and physical nodes are attached.
# For some reason if I use get_tree().get_root().add_child( inst ) those stay invisible.
var root_node: Node setget set_root, get_root

func set_root( root: Node ):
	root_node = root

func get_root():
	return root_node


# create instance
func create( type_name: String ):
	match type_name:
		"cube":
			return cube()
		"plane":
			return plane()
		"capsule_dbg":
			return capsule_dbg()
		"construction":
			return construction()
		"frame_box":
			return frame_box()


func _create( CL ):
	var inst = CL.instance()
	var root = BodyCreator.root_node
	if root != null:
		root.add_child( inst )
	inst.init()
	return inst



func cube():
	var B = load( "res://physics/bodies/cube/cube.tscn" )
	var b = _create( B )
	return b


func plane():
	var B = load( "res://physics/bodies/plane/plane.tscn" )
	var b = _create( B )
	return b


func capsule_dbg():
	var B = load( "res://physics/bodies/capsule_dbg/capsule_dbg.tscn" )
	var b = _create( B )
	return b


func construction():
	var B = load( "res://physics/bodies/construction/construction.tscn" )
	var b = _create( B )
	return b


func frame_box():
	var B = load( "res://physics/bodies/frames/frame_box/frame_box.tscn" )
	var b = _create( B )
	return b
	
