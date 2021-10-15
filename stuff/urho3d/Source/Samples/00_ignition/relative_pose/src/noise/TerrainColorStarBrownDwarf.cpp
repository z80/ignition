// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorStarBrownDwarf>::GetColorFractalName() const { return "StarBrownDwarf"; }

template <>
TerrainColorFractal<TerrainColorStarBrownDwarf>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 5e8, 100.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorStarBrownDwarf>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	Vector3d col;
	n = voronoiscam_octavenoise(GetFracDef(0), 0.6, p) * 0.5;
	if (n > 0.666) {
		n -= 0.666;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.25, .2, .2), Vector3d(.1, .0, .0));
		return col;
	} else if (n > 0.333) {
		n -= 0.333;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.2, .25, .1), Vector3d(.25, .2, .2));
		return col;
	} else {
		n *= 3.0;
		col = interpolate_color(n, Vector3d(1.5, 1.0, 1.0), Vector3d(.2, .25, .1));
		return col;
	}
}


}



