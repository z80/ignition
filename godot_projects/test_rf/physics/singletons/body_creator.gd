
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
		"frame_light":
			return frame_light()
		"part_cylinder":
			return part_cylinder()
		"character_simple":
			return part_character_simple()
		"character_central":
			return part_character_central()
		"reaction_wheels_2m":
			return part_reaction_wheels_2m()
		"thruster_01":
			return part_thruster_01()


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


func frame_light():
	var B = load( "res://physics/bodies/frames/frame_light/frame_light.tscn" )
	var b = _create( B )
	return b


func part_cylinder():
	var B = load( "res://physics/bodies/parts/part_cylinder/part_cylinder.tscn" )
	var b = _create( B )
	return b


func part_character_simple():
	var B = load( "res://physics/bodies/character_simple/character_simple.tscn" )
	var b = _create( B )
	return b


func part_character_central():
	var B = load( "res://physics/bodies/character_central/character_central.tscn" )
	var b = _create( B )
	return b


func part_reaction_wheels_2m():
	var B = load( "res://physics/bodies/parts/2m/reaction_wheels/reaction_wheels_2m.tscn" )
	var b = _create( B )
	return b


func part_thruster_01():
	var B = load( "res://physics/bodies/parts/thrusters/thruster_01/thruster_01.tscn" )
	var b = _create( B )
	return b

