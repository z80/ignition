
extends Part
class_name FuelTank

export(Resource) var fuel_params = preload( "res://physics/parts/fuel_tanks/resource_fuel_tanks.tres" )

export(float) var total_volume = 10.0
var _initial_volume: float = 0.0
var _fuel_left: float    = 0.0


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func init():
	.init()
	_initial_volume = total_volume
	_fuel_left      = total_volume
	



