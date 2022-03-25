
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
		"construction":
			return construction()
		"frame_box":
			return frame_box()
		"frame_light":
			return frame_light()
		"part_cylinder":
			return part_cylinder()
		"reaction_wheels_2m":
			return part_reaction_wheels_2m()
		
		# Thrusters
		"thruster_01":
			return part_thruster_01()
		"thruster_atmospheric":
			return part_thruster_atmospheric()
		"thruster_vacuum":
			return part_thruster_vacuum()
		
		# 2m diameter fuel tanks.
		"fuel_tank_2m_3m":
			return fuel_tank_2m_3m()
		"oxidizer_tank_2m_3m":
			return oxidizer_tank_2m_3m()
		"fuel_tank_2m_5m":
			return fuel_tank_2m_5m()
		"oxidizer_tank_2m_5m":
			return oxidizer_tank_2m_5m()
#		"fuel_tank_2m_3m":
#			return fuel_tank_2m_3m()
#		"oxidizer_tank_2m_3m":
#			return oxidizer_tank_2m_3m()
		
		# Reaction control wheels
		"reaction_control_wheels_2m_m1":
			return reaction_control_wheels_2m_m1()
		
		# Habitats
		"habitat_2m_m1":
			return habitat_2m_1m()
		
		"decoupler_2m_1m":
			return decoupler_2m_1m()
		
		# Characters
#		"character_simple":
#			return part_character_simple()
		"character_central":
			return part_character_central()


func _create( CL ):
	var inst = CL.instance()
	var root = RootScene.get_root_for_bodies()
	inst.init()
	if root != null:
		inst.change_parent( root )
	return inst



func cube():
	var B = load( "res://physics/bodies/cube/cube.tscn" )
	var b = _create( B )
	return b



func construction():
	var B = load( "res://physics/bodies/construction_new/construction.tscn" )
	var b = _create( B )
	return b


func frame_box():
	var B = load( "res://physics/parts/frames/frame_box/frame_box.tscn" )
	var b = _create( B )
	return b


func frame_light():
	var B = load( "res://physics/parts/frames/frame_light/frame_light.tscn" )
	var b = _create( B )
	return b


func part_cylinder():
	var B = load( "res://physics/parts/parts/part_cylinder/part_cylinder.tscn" )
	var b = _create( B )
	return b


#func part_character_simple():
#	var B = load( "res://physics/parts/character_simple/character_simple.tscn" )
#	var b = _create( B )
#	return b


func part_character_central():
	#var B = load( "res://physics/bodies/character_central/character_central.tscn" )
	var B = load( "res://physics/parts/characters/central/central.tscn" )
	var b = _create( B )
	return b


func part_reaction_wheels_2m():
	var B = load( "res://physics/parts/2m/reaction_wheels/reaction_wheels_2m.tscn" )
	var b = _create( B )
	return b


func part_thruster_01():
	var B = load( "res://physics/parts/thrusters/thruster_01/thruster_01.tscn" )
	var b = _create( B )
	return b


func part_thruster_atmospheric():
	var B = load( "res://physics/parts/thrusters/atmospheric/atmospheric.tscn" )
	var b = _create( B )
	return b


func part_thruster_vacuum():
	var B = load( "res://physics/parts/thrusters/vacuum/vacuum.tscn" )
	var b = _create( B )
	return b


func fuel_tank_2m_3m():
	var B = load( "res://physics/parts/fuel_tanks/2m/3m_fuel/fuel_tank_2m_3m.tscn" )
	var b = _create( B )
	return b


func oxidizer_tank_2m_3m():
	var B = load( "res://physics/parts/fuel_tanks/2m/3m_oxidizer/oxidizer_tank_2m_3m.tscn" )
	var b = _create( B )
	return b


func fuel_tank_2m_5m():
	var B = load( "res://physics/parts/fuel_tanks/2m/5m_fuel/fuel_tank_2m_5m.tscn" )
	var b = _create( B )
	return b


func oxidizer_tank_2m_5m():
	var B = load( "res://physics/parts/fuel_tanks/2m/5m_oxidizer/oxidizer_tank_2m_5m.tscn" )
	var b = _create( B )
	return b


#func fuel_tank_2m_3m():
#	var B = load( "res://physics/parts/fuel_tanks/2m/3m_fuel/fuel_tank.tscn" )
#	var b = _create( B )
#	return b
#
#
#func oxidizer_tank_2m_3m():
#	var B = load( "res://physics/parts/fuel_tanks/2m/3m_oxidizer/oxidizer_tank.tscn" )
#	var b = _create( B )
#	return b



func reaction_control_wheels_2m_m1():
	var B = load( "res://physics/parts/reaction_control_wheels/2m/m1/m1.tscn" )
	var b = _create( B )
	return b


func habitat_2m_1m():
	var B = load( "res://physics/parts/habitats/2m/m1/m1.tscn" )
	var b = _create( B )
	return b


func decoupler_2m_1m():
	var B = load( "res://physics/parts/decouplers/2m/m1/m1.tscn" )
	var b = _create( B )
	return b



