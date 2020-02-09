// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {
namespace Ign {

template <>
const char *TerrainHeightFractal<TerrainHeightMountainsCraters>::GetHeightFractalName() const { return "MountainsCraters"; }

template <>
TerrainHeightFractal<TerrainHeightMountainsCraters>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	SetFracDef(0, m_maxHeightInMeters, m_rand.Double(1e6, 1e7));
	Float height = m_maxHeightInMeters * 0.3;
	SetFracDef(1, height, m_rand.Double(4.0, 20.0) * height);
	SetFracDef(2, m_maxHeightInMeters, m_rand.Double(50.0, 100.0) * m_maxHeightInMeters);

	height = m_maxHeightInMeters * 0.3;
	SetFracDef(4, m_maxHeightInMeters, m_rand.Double(100.0, 200.0) * m_maxHeightInMeters);
	SetFracDef(3, height, m_rand.Double(2.5, 3.5) * height);

	SetFracDef(5, m_maxHeightInMeters * 0.05, 8e5, 1000.0);
	SetFracDef(6, m_maxHeightInMeters * 0.05, 1e6, 10000.0);
}

template <>
Float TerrainHeightFractal<TerrainHeightMountainsCraters>::GetHeight(const Vector3d &p) const
{
	Float continents = octavenoise(GetFracDef(0), 0.5, p) - m_sealevel;
	if (continents < 0) return 0;
	Float n = 0.3 * continents;
	Float m = GetFracDef(1).amplitude * ridged_octavenoise(GetFracDef(1), 0.5, p);
	Float distrib = ridged_octavenoise(GetFracDef(4), 0.5, p);
	if (distrib > 0.5) m += 2.0 * (distrib - 0.5) * GetFracDef(3).amplitude * ridged_octavenoise(GetFracDef(3), 0.5 * distrib, p);
	// cliffs at shore
	if (continents < 0.001)
		n += m * continents * 1000.0f;
	else
		n += m;
	n += crater_function(GetFracDef(5), p);
	n += crater_function(GetFracDef(6), p);
	n *= m_maxHeight;
	return (n > 0.0 ? n : 0.0);
}
