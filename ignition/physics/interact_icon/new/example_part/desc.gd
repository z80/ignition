
extends Resource
class_name CreatableDescBase

# A base class for all creatable objects.
# The idea is that folders are parsed automatically.
# If "desc.tres" is met, it is loaded and all descriptions 
# are obtained.

# Actual part resource name and script name are supposed to 
# match the folder name in which they reside.


func _init():
	# Probably here load WIKI description from a file.
	pass


func get_brief_description():
	return "Physics body base type. Here it should be a description shown when its icon is hovered."

func get_wiki_description():
	return "Here it should be full description shown when WIKI page is opened."

func get_group_name():
	return "dynamic_parts"

