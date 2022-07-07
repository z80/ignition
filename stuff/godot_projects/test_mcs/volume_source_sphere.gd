
extends Reference

const cylinder_r: Vector3 = Vector3.ZERO
const cylinder_a: Vector3 = Vector3.FORWARD

var material_index: int    = 0
var material_priority: int = 0
var node_sz: float = 0.25
var radius: float  = 3.0


func value_a( at: Vector3 ):
	var d: float = at.length()
	var r: float = radius
	var ret: float = d - r
	return ret


func value( at: Vector3=Vector3() ):
	var c: float = value_a( at )
	return c


func max_node_size():
	return node_sz


func min_node_size():
	return node_sz


func max_node_size_local( at: Vector3 ):
	return node_sz


func min_node_size_local( at: Vector3 ):
	return node_sz


func material( at: Vector3 ):
	return material_index


func priority():
	return material_priority






