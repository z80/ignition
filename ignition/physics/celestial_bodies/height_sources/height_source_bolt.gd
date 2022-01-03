
extends Object

var _noise: OpenSimplexNoise = null


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
	_noise = OpenSimplexNoise.new()
	_noise.period = 1.0
	_noise.octaves = 1


func height( at: Vector3 ):
	var h: float = noise( at )
	return h



func color( at: Vector3, norm: Vector3, height: float ):
	var ca: Color = Color( 0.2, 0.2, 0.2, 1.0 )
	var cb: Color = Color( 0.75, 0.75, 0.75, 1.0 )
	var c: Color = ca + (cb - ca) * clamp( height / 0.2, 0.0, 1.0 )
	return c



func noise( v: Vector3 ):
	var n: OpenSimplexNoise = _noise
	var f: float = 0.0
	
	f = v.x*v.x + v.y*v.y/16.0 + v.z*v.z/15.0

	var mag: Array = [ 1.0, 0.3, 0.2, 0.1 ]
	var lac: Array = [ 0.3, 2.03, 2.07, 2.11 ]
	for i in range(2):
		v *= lac[i]
		var t: float = mag[i] * n.get_noise_3dv( v )
		f += t

	mag = [ 1.0, 0.3, 0.2, 0.1]
	lac = [ 0.6, 2.03, 2.07, 2.11 ]
	for i in range(4):
		v *= lac[i]
		var r: float = n.get_noise_3dv( v )
		var t: float = mag[i] * r*r
		f += t
		
	
	
	return f
	
	

