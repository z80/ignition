@tool

extends SurfaceSourceBase

@export var override_material: Material = null

var _source_solid: VolumeSourceScriptGd = null

func get_source_solid():
	if _source_solid != null:
		return _source_solid
	
	_source_solid = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://tryouts/celestial_bodies_voxel_2/surface_sources/test_planet/surface_sources/surface_source_funcs_solid.gd" )
	_source_solid.set_script( script )
	_source_solid.bounding_radius = source_radius
	#print( "source_radius: ", source_radius )
	_source_solid.radius = source_radius
	_source_solid.node_sz_max = 5.0
	_source_solid.node_sz_min = 2.0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	_source_solid.se3 = se3
	
	_source_solid.material_index = 0
	
	return _source_solid


var _source_liquid: VolumeSourceScriptGd = null

func get_source_liquid():
	#return null
	
	if _source_liquid != null:
		return _source_liquid
	
	_source_liquid = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://tryouts/celestial_bodies_voxel_2/surface_sources/test_planet/surface_sources/surface_source_funcs_liquid.gd" )
	_source_liquid.set_script( script )
	_source_liquid.bounding_radius = source_radius
	#print( "source_radius: ", source_radius )
	_source_liquid.radius = source_radius
	_source_liquid.node_sz_max = 5.0
	_source_liquid.node_sz_min = 2.0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	_source_liquid.se3 = se3
	
	_source_liquid.material_index = 0
	
	return _source_liquid

