@tool
extends Resource
class_name SurfaceSourceBase

var _bounding_radius: float = 1000.0
@export var bounding_radius: float = 1000.0: get = get_bounding_radius, set = set_bounding_radius

var _source_radius: float = 800.0
@export var source_radius: float = 800.0: get = get_source_radius, set = set_source_radius
@export var radius_km: float: get = get_radius_km, set = set_radius_km

var _orbiting_velocity: float = 100.0
@export var orbiting_velocity: float = 100.0: get = get_orbiting_velocity, set = set_orbiting_velocity

var _g: float = 9.8
@export var g: float: get = get_g, set = set_g

@export var focus_depth: float   = 40.0


@export var drop_materials_solid_here: Material = null: set = _set_drop_materials_solid_here
@export var materials_solid: Array = []

@export var drop_materials_liquid_here: Material = null: set = _set_drop_materials_liquid_here
@export var materials_liquid: Array = []

func _set_drop_materials_solid_here( m: Material ):
	drop_materials_solid_here = null
	materials_solid.push_back( m )
	notify_property_list_changed()


func _set_drop_materials_liquid_here( m: Material ):
	drop_materials_liquid_here = null
	materials_liquid.push_back( m )
	notify_property_list_changed()


func set_bounding_radius( r: float ):
	_bounding_radius = r


func get_bounding_radius():
	return _bounding_radius


func set_source_radius( v: float ):
	_source_radius = v
	notify_property_list_changed()


func get_source_radius():
	return _source_radius


func set_radius_km( r_km: float ):
	_source_radius = r_km * 1000.0
	notify_property_list_changed()


func get_radius_km():
	var ret: float = _source_radius * 0.001
	return ret


func set_orbiting_velocity( v: float ):
	_orbiting_velocity = v
	_g = v*v/_source_radius
	#notify_property_list_changed()


func get_orbiting_velocity():
	return _orbiting_velocity


func set_g( gg: float ):
	_g = gg
	_orbiting_velocity = sqrt(_g*_source_radius)
	#notify_property_list_changed()


func get_g():
	return g


func get_gm():
	var gm: float = _orbiting_velocity * _orbiting_velocity * _source_radius
	return gm

func get_source_solid():
	return null

func get_source_liquid():
	return null
