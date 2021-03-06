// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

// As far as I can tell this is used for the Moon and ONLY the Moon - AndyC

namespace Ign {

template <>
const char *TerrainHeightFractal<TerrainHeightMapped2>::GetHeightFractalName() const { return "Mapped2"; }

template <>
TerrainHeightFractal<TerrainHeightMapped2>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Float TerrainHeightFractal<TerrainHeightMapped2>::GetHeight(const Vector3d &p) const
{
	Float v = BiCubicInterpolation(p);

	v = v * m_heightScaling + m_minh; // v = v*height scaling+min height
	v *= m_invPlanetRadius;

	v += 0.1;
	Float h = 1.5 * v * v * v * ridged_octavenoise(16, 4.0 * v, 4.0, p);
	h += 30000.0 * v * v * v * v * v * v * v * ridged_octavenoise(16, 5.0 * v, 20.0 * v, p);
	h += v;
	h -= 0.09;

	return (h > 0.0 ? h : 0.0);
}

}


