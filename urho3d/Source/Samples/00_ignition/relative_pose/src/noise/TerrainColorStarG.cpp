// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorStarG>::GetColorFractalName() const { return "StarG"; }

template <>
TerrainColorFractal<TerrainColorStarG>::TerrainColorFractal(const PiSourceDesc&body)
    : PiBodySource(body)
{
	Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 8e8, 1000.0);
	SetFracDef(1, height, 7e8, 10000.0);
	SetFracDef(2, height, 4e6, 100.0);
	SetFracDef(3, height, 2e5, 100.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorStarG>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	Vector3d col;
	n = octavenoise(GetFracDef(0), 0.5, p) * 0.5;
	n += voronoiscam_octavenoise(GetFracDef(1), 0.5, p) * 0.5;
	n += octavenoise(GetFracDef(0), 0.5, p) * billow_octavenoise(GetFracDef(1), 0.5, p);
	n += octavenoise(GetFracDef(2), 0.5, p) * 0.5 * Clamp(GetFracDef(0).amplitude - 0.2, 0.0, 1.0);
	n += 15.0 * billow_octavenoise(GetFracDef(0), 0.8, piSimplex(p * 3.142) * p) *
		megavolcano_function(GetFracDef(1), p);
	n *= n * 0.15;
	n = 1.0 - n;
	if (n > 0.666) {
		//n -= 0.666; n *= 3.0;
		//col = interpolate_color(n, Vector3d(1.0, 1.0, 1.0), Vector3d(1.0, 1.0, 1.0) );
		col = Vector3d(1.0, 1.0, 1.0);
		return col;
	} else if (n > 0.333) {
		n -= 0.333;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.6, .6, .0), Vector3d(1.0, 1.0, 1.0));
		return col;
	} else if (n > 0.05) {
		n -= 0.05;
		n *= 3.533;
		col = interpolate_color(n, Vector3d(.8, .8, .0), Vector3d(.6, .6, .0));
		return col;
	} else {
		n *= 20.0;
		col = interpolate_color(n, Vector3d(.02, .0, .0), Vector3d(.8, .8, .0));
		return col;
	}
}


}


