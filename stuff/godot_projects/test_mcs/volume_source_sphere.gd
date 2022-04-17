
extends Reference


func value( at: Vector3=Vector3() ):
	var d: float = at.length()
	var r: float = 10.0
	var ret: float = d - r
	return ret


func max_node_size():
	return 1.0

func min_node_size():
	return 1.0

func max_node_size_at( at: Vector3 ):
	return 1.0






