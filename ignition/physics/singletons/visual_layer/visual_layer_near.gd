
extends Node3D

@export var sun_direction: Basis = Basis.IDENTITY: get = _get_sun_direction, set = _set_sun_direction

# This is actual root for adding visual content.
var root: Node3D = null



# Called when the node enters the scene tree for the first time.
func _ready():
	_get_root()


func _get_root():
	if root == null:
		root = get_node( "Root" )
	
	return root


func _set_sun_direction( b: Basis ):
	root.sun_direction = b


func _get_sun_direction():
	var ret: Basis = root.sun_direction
	return ret


