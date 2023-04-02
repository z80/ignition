
static func frac_def( radius: float, height: float, feature_height_m: float, feature_width_m: float, smallest_octave_sz_m: float ):
	var n: FastNoiseLite = FastNoiseLite.new()
	var amplitude: float = feature_height_m / (height * radius)
	n.period = feature_width_m / radius
	n.fractal_octaves = max(1.0, int(ceil(log(feature_width_m / smallest_octave_sz_m) / log(2.0))))
	n.lacunarity = 2.0;
	return [n, amplitude]

static func octavenoise( n: FastNoiseLite, persistence: float, at: Vector3 ):
	n.persistence = persistence
	var ret: float = n.get_noise_3dv( at )
	ret = persistence * (ret + 1.0) * 0.5
	return ret

static func river_octavenoise( n: FastNoiseLite, persistence: float, at: Vector3 ):
	n.persistence = persistence
	var ret: float = abs( n.get_noise_3dv( at ) )
	ret = persistence * ret
	return ret



