tool
extends Resource
class_name SurfaceSourceBase

export(float) var source_radius = 1000.0 setget set_source_radius, get_source_radius
export(float) var focus_depth   = 40.0
export(float) var identity_distance = 100.0


export(Material) var drop_materials_here = null setget _set_drop_materials_here
export(Array) var materials: Array = []

func _set_drop_materials_here( m: Material ):
	drop_materials_here = null
	materials.push_back( m )
	property_list_changed_notify()




func set_source_radius( v: float ):
	source_radius = v


func get_source_radius():
	return source_radius


func get_source():
	return null
