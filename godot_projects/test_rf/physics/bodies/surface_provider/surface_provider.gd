
extends Body


func init():
	var Visual   = load( "res://physics/bodies/surface_provider/surface_provider_visual.tscn")
	var Physical = load( "res://physics/bodies/surface_provider/surface_provider_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		_physical.set_collision_layer( layer )

func set_vertices( vertices: PoolVector3Array ):
	if _physical != null:
		_physical.set_vertices( vertices )
