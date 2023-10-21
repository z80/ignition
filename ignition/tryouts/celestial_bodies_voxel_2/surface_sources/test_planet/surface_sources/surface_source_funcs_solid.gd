





var material_index: int     = 0
var material_priority: int  = 0
var node_sz_max: float      = 1.25
var node_sz_min: float      = 0.25
var radius: float           = 100.0
var bounding_radius: float  = 100.0
var height: float           = 10.0



var _n_hills: FastNoiseLite = null
var _n_flats: FastNoiseLite = null
var _n_bioms_sep: FastNoiseLite = null
var _n_conts: FastNoiseLite = null

func _init():
	_n_hills     = _noise_hills()
	_n_flats     = _noise_flat_grounds()
	_n_bioms_sep = _noise_bioms_separation()
	_n_conts     = _noise_continents()


func _combination( at: Vector3 ):
	var v_cont: float = _n_conts.get_noise_3dv( at )
	
	var v_bioms_sep: float = _n_bioms_sep.get_noise_3dv( at )
	v_bioms_sep = (v_bioms_sep + 1.0) * 0.5
	
	var coeff_flat: float = v_bioms_sep
	var coeff_hills: float = 1.0 - v_bioms_sep
	var v_flat: float = _n_flats.get_noise_3dv( at ) * 0.5
	if v_flat < 0.0:
		v_flat *= 0.1
	
	var v_hills: float = _n_hills.get_noise_3dv( at ) * 1.7
	if v_hills < 0.0:
		v_hills *= 0.1
	var landscape: float = _softmax( [v_flat, v_hills], [coeff_flat, coeff_hills], 10.0 )
	
	# Now separate ocean floor from continents.
	v_bioms_sep = (v_cont + 1.0) * 0.5
	var coeff_ocean: float = 1.0 - v_bioms_sep
	var coeff_continent: float = v_bioms_sep
	
	var v_ocean: float = v_cont
	v_cont = v_cont + landscape
	
	var value: float = _softmax( [v_ocean, v_cont], [coeff_ocean, coeff_continent], 10.0 )
	
	value += pow( abs(at.y), 2.0 ) * 0.5
	return value


func _softmax( values: Array, weights: Array, alpha: float = 2.0 ):
	var qty: int = values.size()
	var num: float = 0.0
	var den: float = 0.0
	for i in range(qty):
		var value = values[i]
		var weight: float = weights[i]
		var e: float = exp(alpha*weight)
		var n: float = value * e
		num += n
		den += e
	
	var ret: float = num / den
	return ret
		


func _noise_hills( seed: int = 8 ):
	var noise: FastNoiseLite = FastNoiseLite.new()
	noise.noise_type = FastNoiseLite.TYPE_SIMPLEX
	noise.seed = seed
	noise.fractal_octaves = 3
	noise.frequency = 9.0
	
	return noise


func _noise_flat_grounds( seed: int = 8 ):
	var noise: FastNoiseLite = FastNoiseLite.new()
	noise.noise_type = FastNoiseLite.TYPE_SIMPLEX
	noise.seed = seed
	noise.fractal_octaves = 3
	noise.frequency = 2.0
	
	return noise


func _noise_bioms_separation( seed: int = 9 ):
	var noise: FastNoiseLite = FastNoiseLite.new()
	noise.noise_type = FastNoiseLite.TYPE_SIMPLEX
	noise.seed = seed
	#noise.fractal_octaves = 8
	noise.frequency = 2.0
	
	return noise
	


func _noise_continents( seed: int = 8 ):
	var noise: FastNoiseLite = FastNoiseLite.new()
	noise.noise_type = FastNoiseLite.TYPE_SIMPLEX
	noise.seed = seed
	#noise.fractal_octaves = 8
	noise.frequency = 0.5
	
	return noise
	





func value( at: Vector3 ):
	var unit: Vector3 = at / radius
	var n: float      = _combination( unit )
	n  *= height/radius
	var r: float = radius * (1.0 + n)
	if r < (radius - 2.0):
		r = radius - 2.0
		
	
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

