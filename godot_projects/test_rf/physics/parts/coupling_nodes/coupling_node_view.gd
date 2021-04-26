
extends Spatial


export(float) var size = 0.5 setget _set_size, _get_size


# Called when the node enters the scene tree for the first time.
func _ready():
	_set_size( size )


func _set_size( sz: float ):
	var s: Spatial = get_node( "PartNode" )
	s.scale = Vector3( sz, sz, sz )
	size = sz


func _get_size():
	return size



