// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGUranus>::GetColorFractalName() const { return "GGUranus"; }

template <>
TerrainColorFractal<TerrainColorGGUranus>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	const Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 3e7, 1000.0);
	SetFracDef(1, height, 9e7, 1000.0);
	SetFracDef(2, height, 8e7, 1000.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorGGUranus>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = 0.5 * ridged_octavenoise(GetFracDef(0), 0.7, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.5 * octavenoise(GetFracDef(1), 0.6, Vector3d(3.142 * p.y_ * p.y_));
	n += 0.2 * octavenoise(GetFracDef(2), 0.5, Vector3d(3.142 * p.y_ * p.y_));
	n /= 2.0;
	n *= n * n;
	return interpolate_color(n, Vector3d(.4, .5, .55), Vector3d(.85, .95, .96));
}
