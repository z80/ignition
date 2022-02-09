
extends Spatial
class_name ExhaustNode


enum ExhaustType {
	A = 0, 
	B = 1, 
	C = 2
}

export(ExhaustType) var exhaust_type = ExhaustType.A
export(float) var exhaust_radius = 1.0
export(float) var pressure_low: float     = 0.0
export(float) var pressure_optimal: float = 0.8e5
export(float) var pressure_high: float    = 1.0e5

var _relative_to_owner: Transform = Transform.IDENTITY

var _exhaust: Spatial = null

export(Resource) var exhausts = preload( "res://physics/parts/thrusters/exhausts/resource_exhausts.tres" )

# Called when the node enters the scene tree for the first time.
func _ready():
	_relative_to_owner = compute_relative_to_owner()
	_create_exhaust()


func compute_relative_to_owner():
	var p: Node = self
	var t: Transform = Transform.IDENTITY
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret



func _compute_relative_to_owner_recursive( n: Node, t: Transform ):
	var s: Spatial = n as Spatial
	if s != null:
		var ct: Transform = s.transform
		t = t * ct
	var ow: Node = self.owner
	if n == ow:
		return t
	
	var p: Node = get_parent()
	var ret: Transform = _compute_relative_to_owner_recursive( p, t )
	return ret


func _create_exhaust():
	if _exhaust != null:
		return
	var path: String = exhausts.get_exhaust_scene( exhaust_type )
	var Exhaust = load( path )
	if Exhaust == null:
		return
	_exhaust = Exhaust.instance()
	add_child( _exhaust )
	_exhaust.scale = Vector3( exhaust_radius, exhaust_radius, exhaust_radius )
	_exhaust.pressure_low     = pressure_low
	_exhaust.pressure_optimal = pressure_optimal
	_exhaust.pressure_high    = pressure_high
	_exhaust.visible = false



func set_exhaust( enabled: bool, power: float, pressure: float ):
	if _exhaust == null:
		return
	var visible: bool = enabled
	_exhaust.set_exhaust( power, pressure )
	_exhaust.visible = visible



func thrust_direction():
	var v: Vector3 = Vector3.UP
	v = _relative_to_owner.basis.xform( v )
	return v

