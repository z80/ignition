
extends Reference

const cylinder_r: Vector3 = Vector3.ZERO
const cylinder_a: Vector3 = Vector3.FORWARD


func value_a( at: Vector3 ):
	var d: float = at.length()
	var r: float = 3.5
	var ret: float = d - r
	return ret


func value_b( at: Vector3 ):
	var d: float = (at - Vector3(0.0, 4.0, 0.0)).length()
	var r: float = 2.5
	var ret: float = d - r
	return ret


func value_c( at: Vector3 ):
	var dr: Vector3   = at - cylinder_r
	var proj: Vector3 = dr.project( cylinder_a )
	var d: float      = ( dr - proj ).length() - 1.3
	return d
	

func value( at: Vector3=Vector3() ):
	var a: float = value_a( at )
	var b: float = value_b( at )
	var c: float = value_c( at )
	var ret: float = max( min(a, b), -c )
	return ret


func max_node_size():
	return 0.5

func min_node_size():
	return 0.5

func max_node_size_at( at: Vector3 ):
	return 0.5






