
extends Body


func _ready():
	init()


func init():
	var Visual   = preload( "res://physics/bodies/capsule_dbg/visual.tscn")
	var Physical = preload( "res://physics/bodies/capsule_dbg/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
	
	
	var t: Transform
	t.origin = Vector3( 0.0, 20.0, 0.0 )
	set_t( t )



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer
