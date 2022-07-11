
extends Reference

var planet_r: float = 10.0
var crater_r: float = 1.0
var crater_h: float = 1.0
var neg_shift: float = 0.8

var node_sz_max: float = 10.0
var node_sz_min: float = 1.0

var material_index: int    = 0
var material_priority: int = 0


func value( at: Vector3=Vector3() ):
	var y: float = crater_h*(1.0 - neg_shift)
	var r0: Vector3 = Vector3( 0.0, planet_r + y, 0.0 )
	var d: float = (at - r0).length() - crater_r
	if d > 0.0:
		return d
	
	var x: float = sqrt( at.x*at.x + at.z*at.z ) / crater_r
	y = planet_r + crater_h * ( 1.0 / ( 1.0 + x*x ) - neg_shift )
	var lower_dist: float = (at.y - y)
	var c: Vector3 = Vector3( 0.0, planet_r + crater_h*(1.0 - neg_shift), 0.0 )
	var sphere_dist: float = (at - c).length() - crater_r
	
	d = max( lower_dist, -sphere_dist )
	return d


func max_node_size():
	return node_sz_max


func min_node_size():
	return node_sz_min


func max_node_size_local( at: Vector3 ):
	return node_sz_max


func min_node_size_local( at: Vector3 ):
	return node_sz_min


func material( at: Vector3 ):
	return material_index


func priority():
	return material_priority


