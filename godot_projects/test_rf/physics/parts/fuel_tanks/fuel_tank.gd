
extends Part
class_name FuelTank

enum FuelType {LIQUID_FUEL=0, LIQUID_OXIDIZER=1, SOLID_FUEL=2}

export(Resource) var fuel_params = preload( "res://physics/parts/fuel_tanks/resource_fuel_tanks.tres" )

export(FuelType) var fuel_type = FuelType.LIQUID_FUEL
export(float) var radius = 1.0
export(float) var height = 5.0

var initial_volume_percent: float = 100.0
var _fuel_left: float    = 0.0



# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()




func init():
	.init()
	set_volume_percent( initial_volume_percent )



func set_volume_percent( percent: float = 100.0 ):
	var full_v: float = full_volume()
	_fuel_left = full_v * percent / 100.0


func full_volume():
	var v: float = height * radius * radius * PI
	return v



func update_inertia():
	var density: float = _fuel_density()
	var m: float = _fuel_left * density
	# Add dry mass.
	m += mass
	
	# And compute inertia based on cynindrical shape.
	var h: float = height
	var r: float = radius
	var Ixx: float = m*h*h/12.0 + m*r*r/4.0
	var Izz: float = Ixx
	var Iyy: float = m*r*r/2.0
	
	if _physical != null:
		_physical.mass = m
		# Don't know how to specify inertia.
		# Can't find a method or property.




func _fuel_density():
	var density: float = 0.0
	if fuel_type == FuelType.LIQUID_FUEL:
		density = fuel_params.liquid_fuel_density
	elif fuel_type == FuelType.LIQUID_OXIDIZER:
		density = fuel_params.liquid_oxidizer_density
	else:
		density = fuel_params.solid_fuel_density
	return density





