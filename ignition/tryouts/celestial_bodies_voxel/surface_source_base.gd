@tool
extends Resource
class_name SurfaceSourceBase

@export var bounding_radius: float = 1000.0: get = get_bounding_radius, set = set_bounding_radius
@export var source_radius: float = 800.0: get = get_source_radius, set = set_source_radius
@export var radius_km: float: get = get_radius_km, set = set_radius_km
@export var orbiting_velocity: float: get = get_orbiting_velocity, set = set_orbiting_velocity
@export var g: float: get = get_g, set = set_g
#export(float) var gm setget , get_gm

@export var focus_depth: float   = 40.0


@export var drop_materials_solid_here: Material = null: set = _set_drop_materials_solid_here
@export var materials_solid: Array: Array = []

@export var drop_materials_liquid_here: Material = null: set = _set_drop_materials_liquid_here
@export var materials_liquid: Array: Array = []


func _set_drop_materials_solid_here( m: Material ):
	drop_materials_solid_here = null
	materials_solid.push_back( m )
	notify_property_list_changed()


func _set_drop_materials_liquid_here( m: Material ):
	drop_materials_liquid_here = null
	materials_liquid.push_back( m )
	notify_property_list_changed()


func set_bounding_radius( r: float ):
	bounding_radius = r


func get_bounding_radius():
	return bounding_radius


func set_source_radius( v: float ):
	source_radius = v
	notify_property_list_changed()


func get_source_radius():
	return source_radius


func set_radius_km( r_km: float ):
	source_radius = r_km * 1000.0
	notify_property_list_changed()


func get_radius_km():
	var ret: float = source_radius * 0.001
	return ret


func set_orbiting_velocity( v: float ):
	orbiting_velocity = v
	g = v*v/source_radius
	notify_property_list_changed()


func get_orbiting_velocity():
	return orbiting_velocity


func set_g( gg: float ):
	g = gg
	orbiting_velocity = sqrt(g*source_radius)
	notify_property_list_changed()


func get_g():
	return g


func get_gm():
	var gm: float = orbiting_velocity * orbiting_velocity * source_radius
	return gm

func get_source_solid():
	return null

func get_source_liquid():
	return null
