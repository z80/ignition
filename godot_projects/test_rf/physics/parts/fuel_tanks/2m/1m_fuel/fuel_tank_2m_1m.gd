
extends FuelTank


func init():
	var Visual   = load( "res://physics/parts/fuel_tanks/2m/1m_fuel/visual.tscn")
	var Physical = load( "res://physics/parts/fuel_tanks/2m/1m_fuel/physical.tscn" )
	
	VisualType   = Visual
	PhysicalType = Physical

	.init()
