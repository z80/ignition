
extends Decoupler


func init():
	var Visual   = load( "res://physics/parts/decouplers/2m/m1/visual.tscn")
	var Physical = load( "res://physics/parts/decouplers/2m/m1/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
	
	# Remember decoupling node.
	var dn: Node = _visual.get_decoupling_node()
	decoupling_node_path = dn.get_path()

