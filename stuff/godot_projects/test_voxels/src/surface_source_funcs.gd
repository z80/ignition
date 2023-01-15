
extends Reference




var material_index: int     = 0
var material_priority: int  = 0
var node_sz_max: float      = 2.0
var node_sz_min: float      = 0.1
var radius: float           = 8.0
var bounding_radius: float  = 10.0
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


func value( at: Vector3 ):
	var SCALE: float = 3000.0
	var unit: Vector3 = at / self.radius
	var dr: float = noise.get_noise_3dv( unit )
	at += at.normalized() * dr * 0.3
	
	var d: float   = at.length()
	var ret: float = d - self.radius
	
#	# Add an empty cylinder
#	var d2: float = sqrt(at.y*at.y + at.x*at.x) - 100.0
#
#	ret = max(ret, -d2)
#
#	var small_radius: float = radius * 0.4
#	d2 = max( max( abs(at.x), abs(at.y) ), abs(at.z) ) - small_radius
#	ret = max(ret, -d2)
	
	return ret



func max_node_size():
	return node_sz_max


func min_node_size():
	return node_sz_min


func max_node_size_local( at: Vector3 ):
	return node_sz_max


func min_node_size_local( at: Vector3 ):
	return node_sz_min

func set_bounding_radius( r: float ):
	bounding_radius = r

func get_bounding_radius():
	return bounding_radius

func material( at: Vector3 ):
	return material_index


func priority():
	return material_priority


