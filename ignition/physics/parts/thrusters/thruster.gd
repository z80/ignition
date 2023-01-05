
extends Part
class_name Thruster

var ClosestCelestialBody = preload( "res://physics/utils/closest_celestial_body.gd" )

export(bool) var debug_mode = false

enum FuelType {LIQUID_FUEL=0, SOLID_FUEL=1}
export(FuelType) var fuel_type = FuelType.LIQUID_FUEL

export(float) var throttle = 0.0
export(bool) var can_shut_down = true
export(int) var restarts_qty = -1
var restarts_left: int = -1

export(float) var optimal_thrust_air_pressure = 0.8e5
export(float) var bad_thrust_air_pressure     = 0.4e5
export(float) var thrust_min_optimal = 1.0
export(float) var thrust_max_optimal = 2.0
export(float) var thrust_min_bad = 1.0
export(float) var thrust_max_bad = 2.0
# Units of volume per second.
export(float) var fuel_consumption_min = 0.2
export(float) var fuel_consumption_max = 0.5

# Sounds.
export(String) var sound_start = ""
export(String) var sound_loop  = ""
export(String) var sound_stop  = ""
export(String) var sound_failed_start = ""


var _exhaust_nodes: Array = []

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
	
	var PartControlGroups = load( "res://physics/parts/part_control_groups.gd" )
	control_group = PartControlGroups.ControlGroup._1



func process_inner( _delta: float ):
	.process_inner( _delta )
	_process_fuel( _delta )
	# Need to do it all the time because when 
	# RF switch/jump happens physical object is destroyed and 
	# created again. Due to that need to continuoulsy assign it.
	# Also, atmospheric pressure changes.
	_setup_thrust()




func _traverse_exhaust_nodes():
	_traverse_exhaust_nodes_recursive( _visual )



func _traverse_exhaust_nodes_recursive( p: Node ):
	var s: Spatial = p as Spatial
	if s != null:
		var exhaust_node: ExhaustNode = s as ExhaustNode
		if exhaust_node != null:
			_exhaust_nodes.push_back( exhaust_node )
	
	var qty: int = p.get_child_count()
	for i in range( qty ):
		var ch: Node = p.get_child( i )
		_traverse_exhaust_nodes_recursive( ch )
	




func gui_classes( mode: Array ):
	var classes = []
	var common_classes = .gui_classes( mode )
	for cl in common_classes:
		classes.push_back( cl )
	
	var empty: bool = mode.empty()
	if empty:
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
			play_sound( sound_failed_start )
			return
		
		if not debug_mode:
			if (restarts_qty > 0):
				if (not _ignited) and en:
					restarts_left -= 1
				if restarts_left < 0:
					return
	
	
	if not en and _ignited:
		stop_sound( sound_loop )
	
	var started: bool = (not _ignited) and en
	_ignited = en
	
	_setup_thrust()
	
	if started:
		play_sound( sound_loop, sound_start, sound_stop )



func get_ignited():
	return _ignited



func set_throttle( th: float ):
	throttle = th
	
	_setup_thrust()



func get_throttle():
	return throttle






func _process_fuel( _delta: float ):
	if not _ignited:
		return
	if debug_mode:
		return
	if fuel_type == FuelType.LIQUID_FUEL:
		_process_liquid_fuel( _delta )
	elif fuel_type == FuelType.SOLID_FUEL:
		_process_solid_fuel( _delta )



func _process_liquid_fuel( _delta: float ):
	if _liquid_fuel_tank == null:
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
		return

	if _liquid_oxidizer_tank == null:
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
		return
	
	var v: float = throttle * (fuel_consumption_max - fuel_consumption_min) + fuel_consumption_min
	var dv: float = _delta * v
	var fuel_left: float = _liquid_fuel_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_liquid_fuel_tank._fuel_left = 0.0
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
	_liquid_fuel_tank._fuel_left = fuel_left
	_liquid_fuel_tank._mass_changed = true
	
	fuel_left = _liquid_oxidizer_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_liquid_oxidizer_tank._fuel_left = 0.0
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
	_liquid_oxidizer_tank._fuel_left = fuel_left
	_liquid_oxidizer_tank._mass_changed = true



func _process_solid_fuel( _delta: float ):
	if _solid_fuel_tank == null:
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
		return
	
	var v: float = throttle * (fuel_consumption_max - fuel_consumption_min) + fuel_consumption_min
	var dv: float = _delta * v
	var fuel_left = _solid_fuel_tank._fuel_left - dv
	if fuel_left <= 0.0:
		_solid_fuel_tank._fuel_left = 0.0
		_ignited = false
		stop_sound( sound_loop )
		set_throttle( 0.0 )
	_solid_fuel_tank._fuel_left = fuel_left
	_solid_fuel_tank._mass_changed = true





func _check_fuel_available():
	_find_fuel_tanks()
	var ret: bool = false
	if debug_mode:
		return true
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
			var pressure: float = 0.0
			var cb: CelestialBody = ClosestCelestialBody.closest_celestial_body( self )
			if cb != null:
				var cs: CelestialSurfaceVoxel = cb as CelestialSurfaceVoxel
				if cs != null:
					var se3: Se3Ref = self.relative_to( cs )
					pressure = cs.air_pressure( se3 )
			
			var thrust: float
			if _ignited:
				thrust = throttle
			
			else:
				thrust = 0.0
			
			for en in _exhaust_nodes:
				en.set_exhaust( _ignited, thrust, pressure )
			
			var d_pressure_relative: float = (pressure - optimal_thrust_air_pressure)/(bad_thrust_air_pressure - optimal_thrust_air_pressure)
			var abs_dpressure_relative: float = abs( d_pressure_relative )
			abs_dpressure_relative = clamp( abs_dpressure_relative, 0.0, 1.0 )
			# Min and max thrust based on ambient air pressure.
			var thrust_max: float = thrust_max_optimal + (thrust_max_bad - thrust_max_optimal)*abs_dpressure_relative
			var thrust_min: float = thrust_min_optimal + (thrust_min_bad - thrust_min_optimal)*abs_dpressure_relative
			# Thrust based on throttle level.
			p = (thrust_max - thrust_min)*thrust + thrust_min
			if p < thrust_min:
				p = thrust_min
		
		else:
			p = 0.0
			for en in _exhaust_nodes:
				en.set_exhaust( _ignited, 0.0, 0.0 )
		
		if not _exhaust_nodes.empty():
			var ex_node: ExhaustNode = _exhaust_nodes[0]
			var n: Vector3 = ex_node.thrust_direction()
			var se3: Se3Ref = self.get_se3()
			var q: Quat = se3.q
			n = q.xform( n )
			var thrust: Vector3 = n * p
			_physical.thrust = thrust
			DDD.important()
			DDD.print( "thrust_assigned: " + str(thrust), -1.0, "thrust_assigned" )



# Find the closest fuel tank.
func _find_fuel_tanks():
	if fuel_type == FuelType.LIQUID_FUEL:
		_find_liquid_fuel_tank()
	elif fuel_type == FuelType.SOLID_FUEL:
		_find_solid_fuel_tank()




func _find_liquid_fuel_tank():
	var visited: Array = []
	_liquid_fuel_tank = _find_closest_liquid_fuel_tank( self, visited )
	if _liquid_fuel_tank != null:
		FuelTank.find_liquid_fuel_tanks( _liquid_fuel_tank )
	visited = []
	_liquid_oxidizer_tank = _find_closest_liquid_oxidizer_tank( self, visited )
	if _liquid_oxidizer_tank != null:
		FuelTank.find_liquid_oxidizer_tanks( _liquid_oxidizer_tank )



func _find_solid_fuel_tank():
	var visited: Array = []
	_solid_fuel_tank = _find_closest_solid_fuel_tank( self, visited )
	if _solid_fuel_tank != null:
		FuelTank.find_solid_fuel_tanks( _solid_fuel_tank )



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
	var atts: Array = part.get_attachments()
	for node in atts:
		var n: CouplingAttachment = node
		var other_part: Part = n.attachment_b.get_part()
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
	var atts: Array = part.get_attachments()
	for node in atts:
		var n: CouplingAttachment = node
		var other_part: Part = n.attachment_b.get_part()
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
	var atts: Array = part.get_attachments()
	for node in atts:
		var n: CouplingAttachment = node
		var other_part: Part = n.attachment_b.get_part()
		var ret: Part = _find_closest_solid_fuel_tank( other_part, visited )
		if ret != null:
			return ret
	
	return null





