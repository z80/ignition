
extends Habitat


func init():
	var Visual   = load( "res://physics/parts/habitats/2m/m1/visual.tscn")
	var Physical = load( "res://physics/parts/habitats/2m/m1/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()

