
extends Resource
class_name CreatableTechnology


@export var tech_name: String = ""
@export var tech_name_visual: String = "Human readable technology name goes here"
@export var tech_name_parent: String = ""

@export var icon: Resource = null: get = get_icon

# This one should be overriden in implementations.
func get_icon():
	return icon
