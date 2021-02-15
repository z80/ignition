
extends Body

var inv_surface_relative_to_rf: Se3Ref = null

func init():
	var Visual   = load( "res://physics/bodies/surface_provider/surface_provider_visual.tscn")
	var Physical = load( "res://physics/bodies/surface_provider/surface_provider_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()



func set_collision_layer( layer ):
	if _physical:
		_physical.set_collision_layer( layer )


func set_vertices( vertices: PoolVector3Array, surface_relative_to_rf: Se3Ref ):
	self.inv_surface_relative_to_rf = surface_relative_to_rf.inverse()
	if _physical != null:
		_physical.transform = Transform.IDENTITY
		_physical.set_vertices( vertices )


func update_vertices( se3: Se3Ref ):
	var cur: Se3Ref = se3.mul( inv_surface_relative_to_rf )
	_physical.transform = cur.transform

