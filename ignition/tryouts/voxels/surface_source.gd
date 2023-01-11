
tool
extends Resource

var _source: VolumeSourceScriptGd = null

func get_source():
	if _source != null:
		return _source
	
	_source = VolumeSourceScriptGd.new()
	var script: Resource = preload( "res://tryouts/voxels/surface_source_funcs.gd" )
	_source.set_script( script )
	
	return _source



