
extends Resource

@export var exhaust_00: String = "res://physics/parts/thrusters/exhausts/exhaust_00/exhaust_00.tscn"
@export var exhaust_01: String = "res://physics/parts/thrusters/exhausts/exhaust_01/exhaust.tscn"

func get_exhaust_scene( type: int ):
	if type == 0:
		return exhaust_00
	if type == 1:
		return exhaust_01
	
	return exhaust_01


