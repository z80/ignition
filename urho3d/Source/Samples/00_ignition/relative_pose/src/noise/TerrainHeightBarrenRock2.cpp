// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {
namespace Ign {

// Strange world, looks like its been hit by thousands of years of erosion.
// Could be used as a basis for terrains that should have erosion.

template <>
const char *TerrainHeightFractal<TerrainHeightBarrenRock2>::GetHeightFractalName() const { return "Barren Rock 2"; }

template <>
TerrainHeightFractal<TerrainHeightBarrenRock2>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Float TerrainHeightFractal<TerrainHeightBarrenRock2>::GetHeight(const Vector3d &p) const
{

	Float n = billow_octavenoise(16, 0.3 * octavenoise(8, 0.4, 2.5, p), Clamp(5.0 * ridged_octavenoise(8, 0.377, 4.0, p), 1.0, 5.0), p);

	return (n > 0.0 ? m_maxHeight * n : 0.0);
}
