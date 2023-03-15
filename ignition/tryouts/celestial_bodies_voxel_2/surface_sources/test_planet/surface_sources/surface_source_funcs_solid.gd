





var material_index: int     = 0
var material_priority: int  = 0
var node_sz_max: float      = 1.25
var node_sz_min: float      = 0.25
var radius: float           = 100.0
var bounding_radius: float  = 100.0
var noise: OpenSimplexNoise = null
var height: float           = 3.0



func _init():
	noise = OpenSimplexNoise.new()
	noise.octaves = 8
	noise.period = 3.0
	noise.persistence = 0.8


func init_seed( v ):
	var value: int = int( v )
	noise.seed = value


func value( at: Vector3 ):
	var unit: Vector3 = at / radius
	var n: float      = noise.get_noise_3dv( unit )
	n  *= height/radius
	var r: float = radius * (1.0 + n)
	
	var d: float   = at.length()
	var ret: float = d - r
	
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


func material( at: Vector3 ):
	return material_index


func priority():
	return material_priority


func set_bounding_radius( r: float ):
	bounding_radius = r

func get_bounding_radius():
	return bounding_radius

