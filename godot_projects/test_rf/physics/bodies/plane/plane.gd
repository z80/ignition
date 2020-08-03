extends Body


func init():
	var Visual   = preload( "res://physics/bodies/plane/visual.tscn" )
	var Physical = preload( "res://physics/bodies/plane/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()







func set_collision_layer( layer ):
	if _physical:
		var rb = _physical.get_child( 0 )
		rb.collision_layer = layer
		rb.collision_mask  = layer

