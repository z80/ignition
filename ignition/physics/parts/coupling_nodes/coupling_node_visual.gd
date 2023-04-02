
extends Node3D

@export var node_material: Resource = null
@export var surf_material: Resource = null
@export var size: float = 0.5: get = _get_size, set = _set_size
@export var surface: bool = false: get = _get_surface, set = _set_surface


# Called when the node enters the scene tree for the first time.
func _ready():
	_set_size( size )


func _set_size( sz: float ):
	var s: Node3D = get_node( "PartNode" )
	s.scale = Vector3( sz, sz, sz )
	size = sz


func _get_size():
	return size


func _set_surface( en: bool ):
	surface = en
	var mi: MeshInstance3D = get_node( "PartNode" )
	if en:
		mi.material_override = surf_material
		
	else:
		mi.material_override = node_material


func _get_surface():
	return surface


