
extends Body

func init():
	var Visual   = preload( "res://physics/bodies/construction/visual.tscn")
	var Physical = preload( "res://physics/bodies/construction/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()

