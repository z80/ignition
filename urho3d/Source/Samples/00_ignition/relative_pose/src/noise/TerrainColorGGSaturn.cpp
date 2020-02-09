// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {
namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGSaturn>::GetColorFractalName() const { return "GGSaturn"; }

template <>
TerrainColorFractal<TerrainColorGGSaturn>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	const Float height = m_maxHeightInMeters * 0.1;
	//spot + clouds
	SetFracDef(0, height, 3e7, 10.0);
	SetFracDef(1, height, 9e7, 1000.0);
	SetFracDef(2, height, 8e7, 100.0);
	//spot boundary
	SetFracDef(3, height, 3e7, 10000000.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorGGSaturn>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = 0.4 * ridged_octavenoise(GetFracDef(0), 0.7, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.4 * octavenoise(GetFracDef(1), 0.6, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.3 * octavenoise(GetFracDef(2), 0.5, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.8 * octavenoise(GetFracDef(0), 0.7, Vector3d(p * p.y_ * p.y_));
	n += 0.5 * ridged_octavenoise(GetFracDef(1), 0.7, Vector3d(p * p.y_ * p.y_));
	n /= 2.0;
	n *= n * n;
	n += billow_octavenoise(GetFracDef(0), 0.8, Vector3d(piSimplex(p * 3.142) * p)) *
		megavolcano_function(GetFracDef(3), p);
	return interpolate_color(n, Vector3d(.69, .53, .43), Vector3d(.99, .76, .62));
}
