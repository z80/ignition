
extends Part
class_name Thruster

enum FuelType {LIQUID_FUEL=0, SOLID_FUEL=1}
export(FuelType) var fuel_type = FuelType.LIQUID_FUEL

export(float) var throttle = 0.0
export(bool) var can_shut_down = true
export(int) var restarts_qty = -1
var restarts_left: int = -1

export(float) var thrust_min_atm = 1.0
export(float) var thrust_max_atm = 2.0
export(float) var thrust_min_vac = 1.0
export(float) var thrust_max_vac = 2.0
# Units of volume per second.
export(float) var fuel_consumption_min = 0.2
export(float) var fuel_consumption_max = 0.5

var _exhaust_node: ExhaustNode = null

var _ignited: bool = false

var _liquid_fuel_tank: FuelTank     = null
var _liquid_oxidizer_tank: FuelTank = null
var _solid_fuel_tank: FuelTank      = null


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func init():
	.init()
	restarts_left = restarts_qty
	_ignited      = false
	_traverse_exhaust_nodes()



func process_inner( _delta: float ):
	.process_inner( _delta )
	_process_fuel( _delta )




func _traverse_exhaust_nodes():
	_traverse_exhaust_nodes_recursive( _visual )



func _traverse_exhaust_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var exhaust_node: ExhaustNode = s as ExhaustNode
		if exhaust_node != null:
			_exhaust_node = exhaust_node
			return true
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		var ret: bool = _traverse_exhaust_nodes_recursive( ch )
		if ret:
			return true
	
	return false




func gui_classes( mode: String = "" ):
	var classes = []
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	if mode.length() < 1:
		var Status = load( "res://physics/parts/thrusters/gui_elements/gui_thruster_status.tscn" )
		classes.push_back( Status )
	
	return classes




func process_user_input_group( input: Dictionary ):
	if _physical == null:
		return
	
	if input.has( "ui_space" ):
		var inp = input[ "ui_space" ]
		if inp.pressed:
			var en: bool = get_ignited()
			en = not en
			set_ignited( en )
		
	if input.has( "ui_z" ):
		var inp = input[ "ui_z" ]
		if inp.pressed:
			var th: float = get_throttle()
			th -= 0.1
			if th < 0.0:
				th = 0.0
			set_throttle( th )
		
	if input.has( "ui_x" ):
		var inp = input[ "ui_x" ]
		if inp.pressed:
			var th: float = get_throttle()
			th += 0.1
			if th > 1.0:
				th = 1.0
			set_throttle( th )






func set_ignited( en: bool ):
	if en:
		# If fuel is not available, just return.
		var ok: bool = _check_fuel_available()
		if not ok:
			return
		
		if restarts_qty > 0:
			if (not _ignited) and en:
				restarts_left -= 1
			if restarts_left < 0:
				return
	
	
	
	_ignited = en
	if _exhaust_node != null:
		var t: float = throttle
		if not _ignited:
			t = 0.0
		_exhaust_node.set_exhaust( t, 1.0 )
	
	_setup_thrust()



func get_ignited():
	return _ignited



func set_throttle( th: float ):
	throttle = th
	if (_exhaust_node != null):
		var t: float = throttle
		if not _ignited:
			t = 0.0
		_exhaust_node.set_exhaust( t, 1.0 )
	
	_setup_thrust()



func get_throttle():
	return throttle



func activate( root_call: bool = true ):
	.activate( root_call )



func _process_fuel( _delta: float ):
	if not _ignited:
		return
	if fuel_type == FuelType.LIQUID_FUEL:
		_process_liquid_fuel( _delta )
	elif fuel_type == FuelType.SOLID_FUEL:
		_process_solid_fuel( _delta )



func _process_liquid_fuel( _delta: float ):
	if _liquid_fuel_tank == null:
		_ignited = false
		return

	if _liquid_oxidizer_tank == null:
		_ignited = false
		return
	
	var v: float = throttle * (fuel_consumption_max - fuel_consumption_min) + fuel_consumption_min
	var dv: float = _delta * v
	var fuel_left: float = _liquid_fuel_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_liquid_fuel_tank._fuel_left = 0.0
		_ignited = false
	_liquid_fuel_tank._mass_changed = true
	
	fuel_left = _liquid_oxidizer_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_liquid_oxidizer_tank._fuel_left = 0.0
		_ignited = false
	_liquid_oxidizer_tank._mass_changed = true



func _process_solid_fuel( _delta: float ):
	if _solid_fuel_tank == null:
		_ignited = false
		return
	
	var v: float = throttle * (fuel_consumption_max - fuel_consumption_min) + fuel_consumption_min
	var dv: float = _delta * v
	var fuel_left = _solid_fuel_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_solid_fuel_tank._fuel_left = 0.0
		_ignited = false
	_solid_fuel_tank._mass_changed = true





func _check_fuel_available():
	_find_fuel_tanks()
	var ret: bool = false
	if fuel_type == FuelType.LIQUID_FUEL:
		ret = _check_liquid_fuel_available()
	elif fuel_type == FuelType.SOLID_FUEL:
		ret = _check_solid_fuel_available()
	return ret


func _check_liquid_fuel_available():
	if _liquid_fuel_tank == null:
		return false
	if _liquid_oxidizer_tank == null:
		return false
	if _liquid_fuel_tank._fuel_left <= 0.0:
		return false
	if _liquid_oxidizer_tank._fuel_left <= 0.0:
		return false
	return true


func _check_solid_fuel_available():
	if _solid_fuel_tank == null:
		return false
	if _solid_fuel_tank._fuel_left <= 0.0:
		return false
	return true




func _setup_thrust():
	if _physical != null:
		var p: float
		if _ignited:
			p = (thrust_max_atm - thrust_min_atm)*throttle + thrust_min_atm
		else:
			p = 0.0
		var n: Vector3 = _exhaust_node.thrust_direction()
		var se3: Se3Ref = self.get_se3()
		var q: Quat = se3.q
		n = q.xform( n )
		var thrust: Vector3 = n * p
		_physical.thrust = thrust



# Find the closest fuel tank.
func _find_fuel_tanks():
	if fuel_type == FuelType.LIQUID_FUEL:
		_find_liquid_fuel_tank()
	elif fuel_type == FuelType.SOLID_FUEL:
		_find_solid_fuel_tank()




func _find_liquid_fuel_tank():
	var visited: Array = []
	_liquid_fuel_tank = _find_closest_liquid_fuel_tank( self, visited )
	visited = []
	_liquid_oxidizer_tank = _find_closest_liquid_oxidizer_tank( self, visited )



func _find_solid_fuel_tank():
	var visited: Array = []
	_solid_fuel_tank = _find_closest_solid_fuel_tank( self, visited )



static func _find_closest_liquid_fuel_tank( part: Part, visited: Array ):
	var has: bool = visited.has( part )
	if has:
		return null
	
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelTank.FuelType.LIQUID_FUEL)
		if right_fuel:
			return ft
	
	var conducts: bool = (ft != null) or part.conducts_liquid_fuel
	if not conducts:
		return null
	
	visited.push_back( part )
	for node in part.stacking_nodes:
		var n: CouplingNodeStacking = node
		var connected: bool = n.connected()
		if not connected:
			continue
		var other_part: Part = n.node_b.part
		var ret: Part = _find_closest_liquid_fuel_tank( other_part, visited )
		if ret != null:
			return ret
	
	return null




static func _find_closest_liquid_oxidizer_tank( part: Part, visited: Array ):
	var has: bool = visited.has( part )
	if has:
		return null
	
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelTank.FuelType.LIQUID_OXIDIZER)
		if right_fuel:
			return ft
	
	var conducts: bool = (ft != null) or part.conducts_liquid_fuel
	if not conducts:
		return null
	
	visited.push_back( part )
	for node in part.stacking_nodes:
		var n: CouplingNodeStacking = node
		var connected: bool = n.connected()
		if not connected:
			continue
		var other_part: Part = n.node_b.part
		var ret: Part = _find_closest_liquid_oxidizer_tank( other_part, visited )
		if ret != null:
			return ret
	
	return null


static func _find_closest_solid_fuel_tank( part: Part, visited: Array ):
	var has: bool = visited.has( part )
	if has:
		return null
	
	var ft: FuelTank = part as FuelTank
	if ft != null:
		var right_fuel: bool = (ft.fuel_type == FuelTank.FuelType.SOLID_FUEL)
		if right_fuel:
			return ft
	
	var conducts: bool = (ft != null) or part.conducts_solid_fuel
	if not conducts:
		return null
	
	visited.push_back( part )
	for node in part.stacking_nodes:
		var n: CouplingNodeStacking = node
		var connected: bool = n.connected()
		if not connected:
			continue
		var other_part: Part = n.node_b.part
		var ret: Part = _find_closest_solid_fuel_tank( other_part, visited )
		if ret != null:
			return ret
	
	return null





