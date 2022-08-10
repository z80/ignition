tool
extends Resource
class_name SurfaceSourceBase

export(float) var source_dimensions = 0.0 setget set_source_dimensions, get_source_dimensions

export(Material) var drop_materials_here = null setget _set_drop_materials_here
export(Array) var materials: Array = []

func _set_drop_materials_here( m: Material ):
	drop_materials_here = null
	materials.push_back( m )
	property_list_changed_notify()


func set_source_dimensions( v: float ):
	source_dimensions = v


func get_source_dimensions():
	return source_dimensions


func get_source():
	return null
