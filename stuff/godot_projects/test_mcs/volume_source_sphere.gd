
extends Reference

const cylinder_r: Vector3 = Vector3.ZERO
const cylinder_a: Vector3 = Vector3.FORWARD

var last_priority: int = -1


func value_a( at: Vector3 ):
	var d: float = at.length()
	var r: float = 3.0
	var ret: float = d - r
	return ret


func value( at: Vector3=Vector3() ):
	var c: float = value_a( at )
	return c


func max_node_size():
	return 3.25


func min_node_size():
	return 3.25


func max_node_size_at( at: Vector3 ):
	return 3.25


func material( at: Vector3 ):
	last_priority = 0
	return 0


func priority():
	return last_priority






