
extends Part
class_name Thruster



export(float) var throttle = 0.0
export(bool) var can_shut_down = true
export(int) var restarts_qty = -1
var restarts_left: int = -1

export(float) var thrust_min_atm = 1.0
export(float) var thrust_max_atm = 2.0
export(float) var thrust_min_vac = 1.0
export(float) var thrust_max_vac = 2.0

var _exhaust_node: ExhaustNode = null

var _ignited: bool = false


# Called when the node enters the scene tree for the first time.
func _ready():
	._ready()


func init():
	.init()
	restarts_left = restarts_qty
	_ignited      = false
	_traverse_exhaust_nodes()


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

