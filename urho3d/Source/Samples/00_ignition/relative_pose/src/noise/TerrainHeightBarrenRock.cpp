// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {

// Banded/Ridged pattern mountainous terrain, could pass for desert

template <>
const char *TerrainHeightFractal<TerrainHeightBarrenRock>::GetHeightFractalName() const { return "Barren Rock"; }

template <>
TerrainHeightFractal<TerrainHeightBarrenRock>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	//SetFracDef(0, m_maxHeightInMeters, m_planetRadius);
	// craters
	//SetFracDef(1, 5000.0, 1000000.0, 10000.0*m_fracmult);
}

template <>
Float TerrainHeightFractal<TerrainHeightBarrenRock>::GetHeight(const Vector3d &p) const
{
	/*return std::max(0.0, m_maxHeight * (octavenoise(GetFracDef(0), 0.5, p) +
			GetFracDef(1).amplitude * crater_function(GetFracDef(1), p)));*/
	//fuck the fracdefs, direct control is better:
	Float n = ridged_octavenoise(16, 0.5 * octavenoise(8, 0.4, 2.5, p), Clamp(5.0 * octavenoise(8, 0.257, 4.0, p), 1.0, 5.0), p);

	return (n > 0.0 ? m_maxHeight * n : 0.0);
}

}


