
extends Spatial

export(Resource) var node_material = null
export(Resource) var surf_material = null
export(float) var size = 0.5 setget _set_size, _get_size
export(bool) var surface = false setget _set_surface, _get_surface


# Called when the node enters the scene tree for the first time.
func _ready():
	_set_size( size )


func _set_size( sz: float ):
	var s: Spatial = get_node( "PartNode" )
	s.scale = Vector3( sz, sz, sz )
	size = sz


func _get_size():
	return size


func _set_surface( en: bool ):
	surface = en
	var mi: MeshInstance = get_node( "PartNode" )
	if en:
		mi.material_override = surf_material
		
	else:
		mi.material_override = node_material


func _get_surface():
	return surface


