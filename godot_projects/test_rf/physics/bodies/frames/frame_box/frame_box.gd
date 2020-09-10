
extends Body


func init():
	var Visual   = load( "res://physics/bodies/cube/visual.tscn")
	var Physical = load( "res://physics/bodies/cube/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer
