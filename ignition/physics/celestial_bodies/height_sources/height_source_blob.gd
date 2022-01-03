
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

const COLORS: Array = [ Color( 0.5, 0.5, 0.5, 1.0 ), 
						Color( 0.5, 0.5, 0.2, 1.0 ), 
						Color( 0.5, 0.2, 0.5, 1.0 ), 
						Color( 0.2, 0.5, 0.5, 1.0 ), 
						Color( 0.5, 0.5, 0.5, 1.0 ), 
						Color( 0.5, 0.5, 0.5, 1.0 ) ]
const POSITIONS: Array = [ -2.0, -0.8, -0.3, 0.3, 0.8, 2.0 ]

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
	var n: OpenSimplexNoise = _noise
	var a: Vector3 = Vector3( at.x*0.01, at.y, at.z*0.01 )
	var t: float = n.get_noise_3dv( a )
	t += at.y
	var qty: int = COLORS.size()
	var xa: float = POSITIONS[0]
	for i in range(qty):
		var xb: float = POSITIONS[i+1]
		if t <= xb:
			var x: float = (t - xa)/(xb - xa)
			var ca: Color = COLORS[i]
			var cb: Color = COLORS[i+1]
			
			var c: Color = ca + (cb - ca) * x
			return c
			
		xa = xb
	
	var c: Color = COLORS.back()
	return c



func noise( v: Vector3 ):
	var f: float = (v.x*v.x + v.z*v.z) * 0.1
	
	return f
	
	

