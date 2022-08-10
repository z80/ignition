
tool
extends SurfaceSourceBase

func _ready():
	source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://tryouts/celestial_bodies_voxel/surface_sources/surface_source_funcs.gd" )
	source.set_script( script )
	source.bounding_radius = source_dimensions
	source.radius = source_dimensions - 10.0
	source.node_sz_max = 100.0
	source.node_sz_min = 10.0
	
	var se3: Se3Ref = Se3Ref.new()
	se3.r = Vector3.ZERO
	source.se3 = se3
	
	source.material_index = 0


