
extends Node3D
class_name ExhaustNode


enum ExhaustType {
	A = 0, 
	B = 1, 
	C = 2
}

@export var exhaust_type: ExhaustType = ExhaustType.A
@export var exhaust_radius: float = 1.0
@export var pressure_low: float: float     = 0.0
@export var pressure_optimal: float: float = 0.8e5
@export var pressure_high: float: float    = 1.0e5

var _relative_to_owner: Transform3D = Transform3D.IDENTITY

var _exhaust: Node3D = null

@export var exhausts: Resource = preload( "res://physics/parts/thrusters/exhausts/resource_exhausts.tres" )

# Called when the node enters the scene tree for the first time.
func _ready():
	_relative_to_owner = compute_relative_to_owner()
	_create_exhaust()


func compute_relative_to_owner():
	var p: Node = self
	var t: Transform3D = Transform3D.IDENTITY
	var ret: Transform3D = _compute_relative_to_owner_recursive( p, t )
	return ret



func _compute_relative_to_owner_recursive( n: Node, t: Transform3D ):
	var s: Node3D = n as Node3D
	if s != null:
		var ct: Transform3D = s.transform
		t = t * ct
	var ow: Node = self.owner
	if n == ow:
		return t
	
	var p: Node = get_parent()
	var ret: Transform3D = _compute_relative_to_owner_recursive( p, t )
	return ret


func _create_exhaust():
	if _exhaust != null:
		return
	var path: String = exhausts.get_exhaust_scene( exhaust_type )
	var Exhaust = load( path )
	if Exhaust == null:
		return
	_exhaust = Exhaust.instantiate()
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
	v = _relative_to_owner.basis * (v)
	return v

