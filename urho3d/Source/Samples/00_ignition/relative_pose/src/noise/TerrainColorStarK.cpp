// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorStarK>::GetColorFractalName() const { return "StarK"; }

template <>
TerrainColorFractal<TerrainColorStarK>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 2e9, 100.0);
	SetFracDef(1, height, 7e7, 100.0);
	SetFracDef(2, height, 1e6, 100.0);
	SetFracDef(3, height, 1e3, 100.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorStarK>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	Vector3d col;
	n = octavenoise(GetFracDef(0), 0.6, p) * 0.5;
	n += ridged_octavenoise(GetFracDef(1), 0.7, p) * 0.5;
	n += billow_octavenoise(GetFracDef(0), 0.8, p) * octavenoise(GetFracDef(1), 0.8, p);
	n -= dunes_octavenoise(GetFracDef(2), 0.6, p) * 0.5;
	n += octavenoise(GetFracDef(3), 0.6, p) * 0.5;
	n *= n * 0.3;
	if (n > 0.666) {
		n -= 0.666;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.95, .7, .25), Vector3d(1.0, 1.0, 1.0));
		return col;
	} else if (n > 0.333) {
		n -= 0.333;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.4, .25, .0), Vector3d(.95, .7, .25));
		return col;
	} else if (n > 0.05) {
		n -= 0.05;
		n *= 3.533;
		col = interpolate_color(n, Vector3d(.2, .1, 0), Vector3d(.4, .25, .0));
		return col;
	} else {
		n *= 20.0;
		col = interpolate_color(n, Vector3d(.015, .015, .015), Vector3d(.2, .1, .0));
		return col;
	}
}



}


