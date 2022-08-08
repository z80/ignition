
extends Reference

const cylinder_r: Vector3 = Vector3.ZERO
const cylinder_a: Vector3 = Vector3.FORWARD

var material_index: int     = 0
var material_priority: int  = 0
var node_sz_max: float      = 1.25
var node_sz_min: float      = 0.25
var radius: float           = 3.0
var noise: OpenSimplexNoise = null
var height: float           = 20.0


func _init():
	noise = OpenSimplexNoise.new()
	noise.octaves = 4
	noise.period = 0.2
	noise.persistence = 0.8


func init_seed( v ):
	var value: int = int( v )
	noise.seed = value


func value_a( at: Vector3 ):
	var unit: Vector3 = at / radius
	var n: float      = noise.get_noise_3dv( unit )
	n  *= height/radius
	at *= 1.0 - n
	
	var d: float   = at.length()
	var r: float   = radius
	var ret: float = d - r
	
	return ret


func value( at: Vector3=Vector3() ):
	var c: float = value_a( at )
	return c


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






