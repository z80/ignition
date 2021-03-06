// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {

// Banded/Ridged pattern mountainous terrain, could pass for desert

template <>
const char *TerrainHeightFractal<TerrainHeightAsteroid3>::GetHeightFractalName() const { return "Asteroid3"; }

template <>
TerrainHeightFractal<TerrainHeightAsteroid3>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	SetFracDef(0, m_maxHeightInMeters * 0.05, 1e6, 10000.0);
	const Float height = m_maxHeightInMeters * 0.3;
	SetFracDef(1, height, m_rand.Double(4.0, 20.0) * height);
}

template <>
Float TerrainHeightFractal<TerrainHeightAsteroid3>::GetHeight(const Vector3d &p) const
{
	const Float n = octavenoise(GetFracDef(0), 0.5, p) * ridged_octavenoise(GetFracDef(1), 0.5, p);

	return (n > 0.0 ? m_maxHeight * n : 0.0);
}


}

