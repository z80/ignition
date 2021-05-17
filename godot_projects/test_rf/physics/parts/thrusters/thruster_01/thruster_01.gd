
extends Thruster



func init():
	var Visual   = load( "res://physics/parts/thrusters/thruster_01/thruster_01_visual.tscn")
	var Physical = load( "res://physics/parts/thrusters/thruster_01/thruster_01_physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()






