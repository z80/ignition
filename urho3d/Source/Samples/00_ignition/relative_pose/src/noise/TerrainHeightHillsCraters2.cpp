// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainHeightFractal<TerrainHeightHillsCraters2>::GetHeightFractalName() const { return "HillsCraters2"; }

template <>
TerrainHeightFractal<TerrainHeightHillsCraters2>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	SetFracDef(0, m_maxHeightInMeters, m_rand.Double(1e6, 1e7));
	Float height = m_maxHeightInMeters * 0.6;
	SetFracDef(1, height, m_rand.Double(4.0, 20.0) * height);
	SetFracDef(2, m_maxHeightInMeters, m_rand.Double(50.0, 100.0) * m_maxHeightInMeters);
	SetFracDef(3, m_maxHeightInMeters * 0.07, 11e5, 1000.0);
	SetFracDef(4, m_maxHeightInMeters * 0.05, 98e4, 800.0);
	SetFracDef(5, m_maxHeightInMeters * 0.05, 1e6, 400.0);
	SetFracDef(6, m_maxHeightInMeters * 0.04, 99e4, 200.0);
	SetFracDef(7, m_maxHeightInMeters * 0.05, 12e5, 100.0);
	SetFracDef(8, m_maxHeightInMeters * 0.04, 9e5, 100.0);
}

template <>
Float TerrainHeightFractal<TerrainHeightHillsCraters2>::GetHeight(const Vector3d &p) const
{
	Float continents = octavenoise(GetFracDef(0), 0.5, p) - m_sealevel;
	if (continents < 0) return 0;
	// == TERRAIN_HILLS_NORMAL except river_octavenoise
	Float n = 0.3 * continents;
	Float distrib = river_octavenoise(GetFracDef(2), 0.5, p);
	Float m = GetFracDef(1).amplitude * river_octavenoise(GetFracDef(1), 0.5 * distrib, p);
	// cliffs at shore
	if (continents < 0.001)
		n += m * continents * 1000.0f;
	else
		n += m;
	n += crater_function(GetFracDef(3), p);
	n += crater_function(GetFracDef(4), p);
	n += crater_function(GetFracDef(5), p);
	n += crater_function(GetFracDef(6), p);
	n += crater_function(GetFracDef(7), p);
	n += crater_function(GetFracDef(8), p);
	n *= m_maxHeight;
	return (n > 0.0 ? n : 0.0);
}



}


