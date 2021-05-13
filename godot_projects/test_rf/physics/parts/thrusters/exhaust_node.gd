
extends Spatial
class_name ExhaustNode


enum ExhaustType {
	A = 0, 
	B = 1, 
	C = 2
}

export(ExhaustType) var exhaust_type = ExhaustType.A
export(float) var exhaust_radius = 1.0
export(float) var exhaust_length = 10.0

var _relative_to_owner: Transform = Transform.IDENTITY


# Called when the node enters the scene tree for the first time.
func _ready():
	_relative_to_owner = compute_relative_to_owner()


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
