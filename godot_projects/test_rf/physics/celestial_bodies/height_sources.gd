
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

#void SetFracDef(const unsigned int index, const double featureHeightMeters, const double featureWidthMeters, const double smallestOctaveMeters)
#{
#	assert(index < MAX_FRACDEFS);
#	// feature
#	m_fracdef[index].amplitude = featureHeightMeters / (m_maxHeight * m_planetRadius);
#	m_fracdef[index].frequency = m_planetRadius / featureWidthMeters;
#	m_fracdef[index].octaves = std::max(1, int(ceil(log(featureWidthMeters / smallestOctaveMeters) / log(2.0))));
#	m_fracdef[index].lacunarity = 2.0;
#	//Output("%d octaves\n", m_fracdef[index].octaves); //print
#}




static func height_source( name: String, radius: float, height: float ):
	if name == "00":
		return height_source_00( radius, height )
	if name == "01":
		return height_source_01( radius, height )
	
	return height_source_00( radius, height )



static func height_source_00( radius: float, height: float ):
	var hs: HeightSourceGdRef = HeightSourceGdRef.new()
	var script = preload( "res://physics/celestial_bodies/height_sources/height_source_00.gd" )
	hs.set_script( script )
	hs.init( radius, height )
	return hs


static func height_source_01( radius: float, height: float ):
	var hs: HeightSourceGdRef = HeightSourceGdRef.new()
	var script = preload( "res://physics/celestial_bodies/height_sources/height_source_01.gd" )
	hs.set_script( script )
	hs.init( radius, height )
	return hs







