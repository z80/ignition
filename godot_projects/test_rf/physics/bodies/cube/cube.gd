extends Body


var CubePhysical = preload( "res://physics/bodies/cube/cube_physical.tscn" )

# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func create_physical():
	var c = CubePhysical.instance()
	
	var t: Transform = _state_local.t()
	c.transform = t
	
	get_tree().get_root().add_child( c )
	_physical = c
