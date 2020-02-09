// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {
namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGNeptune>::GetColorFractalName() const { return "GGNeptune"; }

template <>
TerrainColorFractal<TerrainColorGGNeptune>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	const Float height = m_maxHeightInMeters * 0.1;
	//spot boundary
	SetFracDef(0, height, 3e7, 10000000.0);
	//spot
	SetFracDef(1, height, 9e7, 100.0);
	//bands
	SetFracDef(2, height, 8e7, 1000.0);
	SetFracDef(3, height, 1e8, 1000.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorGGNeptune>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = 0.8 * octavenoise(GetFracDef(2), 0.6, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.25 * ridged_octavenoise(GetFracDef(3), 0.55, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.2 * octavenoise(GetFracDef(3), 0.5, Vector3d(3.142 * p.y_ * p.y_));
	//spot
	n += 0.8 * billow_octavenoise(GetFracDef(1), 0.8, Vector3d(noise(p * 3.142) * p)) *
		megavolcano_function(GetFracDef(0), p);
	n /= 2.0;
	n *= n * n;
	return interpolate_color(n, Vector3d(.04, .05, .15), Vector3d(.80, .94, .96));
}
