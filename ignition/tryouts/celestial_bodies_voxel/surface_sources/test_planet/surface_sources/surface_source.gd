
tool
extends SurfaceSourceBase

var _source: VolumeSourceScriptGd = null

func get_source():
	if _source != null:
		return _source
	
	_source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://tryouts/celestial_bodies_voxel/surface_sources/test_planet/surface_sources/surface_source_funcs.gd" )
	_source.set_script( script )
	_source.bounding_radius = source_dimensions
	print( "source_dimensions: ", source_dimensions )
	_source.radius = source_dimensions - 10.0
	_source.node_sz_max = 100.0
	_source.node_sz_min = 50.0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	_source.se3 = se3
	
	_source.material_index = 0
	
	return _source



