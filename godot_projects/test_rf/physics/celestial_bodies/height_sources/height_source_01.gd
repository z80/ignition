
extends Object


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


var noise: Array = []

# Planet dimensions.
var height: float = 0.3
var radius: float = 1.0

# Called when the node enters the scene tree for the first time.
func _init():
	init()
	
func init( radius: float = 1.0, height: float = 0.2 ):
	pass


func height( at: Vector3 ):
	var h: float = 0.0
	return h



func color( at: Vector3, norm: Vector3, height: float ):
	var c: Color
	if height <= 0.0:
		c = Color( 0.7, 0.7, 0.0, 1.0 )
	elif height < 0.1:
		c = Color( 0.0, 0.7, 0.0, 1.0 )
	else:
		c = Color( 0.8, 0.8, 0.8, 1.0 )
	return c




