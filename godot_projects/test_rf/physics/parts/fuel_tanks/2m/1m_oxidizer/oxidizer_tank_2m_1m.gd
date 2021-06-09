
extends FuelTank


func init():
	var Visual   = load( "res://physics/parts/fuel_tanks/2m/1m_oxidizer/visual.tscn")
	var Physical = load( "res://physics/parts/fuel_tanks/2m/1m_oxidizer/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
