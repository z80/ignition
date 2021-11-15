
extends Part
class_name FuelTank

enum FuelType {LIQUID_FUEL=0, LIQUID_OXIDIZER=1, SOLID_FUEL=2}

export(Resource) var fuel_params = preload( "res://physics/parts/fuel_tanks/resource_fuel_tanks.tres" )

export(FuelType) var fuel_type = FuelType.LIQUID_FUEL
export(float) var radius = 1.0
export(float) var height = 5.0

var initial_volume_percent: float = 100.0

# This is in volume.
var _fuel_left: float    = 0.0
var _fuel_initial: float = 0.0
var _mass_changed: bool = false

var accessible_fuel_tanks: Array = []


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()




func init():
	.init()
	set_volume_percent( initial_volume_percent )
	update_inertia()



func process_inner( _delta: float ):
	.process_inner( _delta )
	_equalize_volumes()
	if _mass_changed:
		update_inertia()
		_mass_changed = false



func set_volume_percent( percent: float = 100.0 ):
	var full_v: float = full_volume()
	_fuel_left = full_v * percent / 100.0
	_fuel_initial = _fuel_left


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



func gui_classes( mode: Array ):
	var classes = []
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	var empty: bool = mode.empty()
	if empty:
		var Status = load( "res://physics/parts/fuel_tanks/gui_elements/gui_fuel_tank.tscn" )
		classes.push_back( Status )
	
	return classes



func fill_relative():
	var this_percent: float
	if _fuel_initial > 0.0:
		this_percent = _fuel_left / _fuel_initial
	else:
		this_percent = 0.0
	return this_percent


func _equalize_volumes():
	var consumed: float = _fuel_initial - _fuel_left
	var consumed_enough: bool = (consumed >= fuel_params.delta_volume)
	if not consumed_enough:
		return false
	var this_percent: float = fill_relative()

	for tank in accessible_fuel_tanks:
		if tank == self:
			continue

		var other_percent: float = tank.fill_relative()
		if other_percent <= this_percent:
			continue

		var delta: float = (tank._fuel_left - _fuel_left)*0.5
		var has_enough: bool = (tank._fuel_left > fuel_params.delta_volume)
		if delta >= fuel_params.delta_volume:
			_fuel_left += fuel_params.delta_volume
			tank._fuel_left -= fuel_params.delta_volume
			_mass_changed = true
			tank._mass_changed = true
			# No need to update inertia here.
			# It is done externally anyway each frame.
			#update_inertia()
			#tank.update_inertia()
			# Return immediately after the very first adjustment.
			return true

	return false




static func find_liquid_fuel_tanks( part: Part ):
	var visited: Array = []
	var found: Array = []
	#find_liquid_fuel_tanks_recursive( part, visited, found )
	return found


static func find_liquid_fuel_tanks_recursive( part: Part, visited: Array, found: Array ):
	# First check if was already visited
	var has: bool = visited.has( part )
	if has:
		return

	# Add to visited parts list
	visited.push_back( part )

	# If it is a fuel tans and has the right fuel, add to the found parts list.
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelType.LIQUID_FUEL)
		if right_fuel:
			found.push_back( part )

	# If it is a fuel tank or if it is a part which can conduct fuel, 
	# recursively check all connected parts.
	var conducts: bool = (ft != null) or part.conducts_liquid_fuel
	if not conducts:
		return

	for node in part.stacking_nodes:
		var n: CouplingAttachment = node
		var connected: bool = n.connected()
		var other_part: Part = n.attachment_b.part
		find_liquid_fuel_tanks_recursive( other_part, visited, found )




static func find_liquid_oxidizer_tanks( part: Part ):
	var visited: Array = []
	var found: Array = []
	find_liquid_oxidizer_tanks_recursive( part, visited, found )
	return found


static func find_liquid_oxidizer_tanks_recursive( part: Part, visited: Array, found: Array ):
	# First check if was already visited
	var has: bool = visited.has( part )
	if has:
		return

	# Add to visited parts list
	visited.push_back( part )

	# If it is a fuel tans and has the right fuel, add to the found parts list.
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelType.LIQUID_OXIDIZER)
		if right_fuel:
			found.push_back( part )

	# If it is a fuel tank or if it is a part which can conduct fuel, 
	# recursively check all connected parts.
	var conducts: bool = (ft != null) or part.conducts_liquid_fuel
	if not conducts:
		return

	for node in part.stacking_nodes:
		var n: CouplingAttachment = node
		var connected: bool = n.connected()
		var other_part: Part = n.attachment_b.part
		find_liquid_oxidizer_tanks_recursive( other_part, visited, found )





static func find_solid_fuel_tanks( part: Part ):
	var visited: Array = []
	var found: Array = []
	find_solid_fuel_tanks_recursive( part, visited, found )
	return found


static func find_solid_fuel_tanks_recursive( part: Part, visited: Array, found: Array ):
	# First check if was already visited
	var has: bool = visited.has( part )
	if has:
		return

	# Add to visited parts list
	visited.push_back( part )

	# If it is a fuel tans and has the right fuel, add to the found parts list.
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelType.SOLID_FUEL)
		if right_fuel:
			found.push_back( part )

	# If it is a fuel tank or if it is a part which can conduct fuel, 
	# recursively check all connected parts.
	var conducts: bool = (ft != null) or part.conducts_solid_fuel
	if not conducts:
		return

	for node in part.stacking_nodes:
		var n: CouplingAttachment = node
		var connected: bool = n.connected()
		var other_part: Part = n.attachment_b.part
		find_solid_fuel_tanks_recursive( other_part, visited, found )








