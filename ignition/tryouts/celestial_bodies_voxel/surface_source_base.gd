tool
extends Resource
class_name SurfaceSourceBase

export(float) var bounding_radius = 1000.0 setget set_bounding_radius, get_bounding_radius
export(float) var source_radius = 800.0 setget set_source_radius, get_source_radius
export(float) var radius_km setget set_radius_km, get_radius_km
export(float) var focus_depth   = 40.0
export(float) var identity_distance = 100.0


export(Material) var drop_materials_here = null setget _set_drop_materials_here
export(Array) var materials: Array = []

func _set_drop_materials_here( m: Material ):
	drop_materials_here = null
	materials.push_back( m )
	property_list_changed_notify()


func set_bounding_radius( r: float ):
	bounding_radius = r


func get_bounding_radius():
	return bounding_radius


func set_source_radius( v: float ):
	source_radius = v
	property_list_changed_notify()


func get_source_radius():
	return source_radius


func set_radius_km( r_km: float ):
	source_radius = r_km * 1000.0
	property_list_changed_notify()


func get_radius_km():
	var ret: float = source_radius * 0.001
	return ret

func get_source():
	return null
