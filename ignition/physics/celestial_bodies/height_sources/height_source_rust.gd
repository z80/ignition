
extends Object

var _noise: FastNoiseLite = null


#inline double octavenoise(const fracdef_t &def, const double persistence, const vector3d &p)
#{
#	//assert(persistence <= (1.0 / def.lacunarity));
#	double n = 0;
#	double amplitude = persistence;
#	double frequency = def.frequency;
#	for (int i = 0; i < def.octaves; i++) {
#		n += amplitude * noise(frequency * p);
#		amplitude *= persistence;
#		frequency *= def.lacunarity;
#	}
#	return (n + 1.0) * 0.5;
#}

#struct fracdef_t {
#	fracdef_t() :
#		amplitude(0.0),
#		frequency(0.0),
#		lacunarity(0.0),
#		octaves(0) {}
#	double amplitude;
#	double frequency;
#	double lacunarity;
#	int octaves;
#};

#void SetFracDef(const unsigned int index, 
#				const double featureHeightMeters, 
#				const double featureWidthMeters, 
#				const double smallestOctaveMeters)
#{
#	assert(index < MAX_FRACDEFS);
#	// feature
#	m_fracdef[index].amplitude = featureHeightMeters / (m_maxHeight * m_planetRadius);
#	m_fracdef[index].frequency = m_planetRadius / featureWidthMeters;
#	m_fracdef[index].octaves = std::max(1, int(ceil(log(featureWidthMeters / smallestOctaveMeters) / log(2.0))));
#	m_fracdef[index].lacunarity = 2.0;
#	//Output("%d octaves\n", m_fracdef[index].octaves); //print
#}




const POLAR_CAP: float = 0.85

# Called when the node enters the scene tree for the first time.
func _init():
	init()
	
func init( radius: float = 1.0, height: float = 0.2 ):
	_noise = FastNoiseLite.new()
	_noise.period = 1.0
	_noise.fractal_octaves = 1


func height( at: Vector3 ):
	var h: float = noise( at )
	return h



func color( at: Vector3, norm: Vector3, height: float ):
	var c: Color
	if abs(at.y) > POLAR_CAP:
		c = Color( 0.95, 0.95, 0.95, 1.0 )
		return c
	if height <= 0.0:
		c = Color( 0.2, 0.05, 0.02, 1.0 )
	elif height < 0.2:
		c = Color( 0.3, 0.1, 0.03, 1.0 )
	else:
		c = Color( 0.5, 0.25, 0.35, 1.0 )
	return c



func noise( v: Vector3 ):
	var n: FastNoiseLite = _noise
	var f: float = 0.0
	
	if abs(v.y) > POLAR_CAP:
		var a: Vector3 = v
		var mag: Array = [0.2, 0.1, 0.2, 0.1]
		var lac: Array = [ 1.5, 1.03, 1.07, 1.11 ]
		for i in range(4):
			v *= lac[i]
			var t: float = mag[i] * abs( n.get_noise_3dv( v ) )
			f += t
	
	var mag: Array = [1.0, 0.3, 0.2, 0.1]
	var lac: Array = [ 0.75, 2.03, 2.07, 2.11 ]
	for i in range(2):
		v *= lac[i]
		var t: float = mag[i] * n.get_noise_3dv( v )
		f += t

	mag = [1.0, 0.3, 0.2, 0.1]
	lac = [ 5.0, 2.03, 2.07, 2.11 ]
	for i in range(4):
		v *= lac[i]
		var r: float = n.get_noise_3dv( v )
		var t: float = mag[i] * r*r
		f += t
		
	
	
	return f
	
	

