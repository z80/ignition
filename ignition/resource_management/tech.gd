
extends Resource
class_name CreatableTechnology


export(String) var tech_name = ""
export(String) var tech_name_visual = "Human readable technology name goes here"
export(String) var tech_name_parent = ""

export(Resource) var icon = null setget ,get_icon

# This one should be overriden in implementations.
func get_icon():
	return icon
