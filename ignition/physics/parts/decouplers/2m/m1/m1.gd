
extends Decoupler


func init():
	var Visual   = load( "res://physics/parts/decouplers/2m/m1/visual.tscn")
	var Physical = load( "res://physics/parts/decouplers/2m/m1/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
	
	# Remember decoupling node.
#	decoupling_node = _visual.get_decoupling_node()

