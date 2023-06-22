@tool
extends Node

@export var layer_config: Resource = null
@export var surface_source: Resource = null
@export var drop_foliage_source_here: Resource = null: get = _get_foliage_source, set = _set_foliage_source

@export var foliage_sources: Array = []




func _set_foliage_source( c: Resource ):
	if c != null:
		foliage_sources.push_back( c )
	drop_foliage_source_here = null
	notify_property_list_changed()


func _get_foliage_source():
	return null




