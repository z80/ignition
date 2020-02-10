// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorStarWhiteDwarf>::GetColorFractalName() const { return "StarWhiteDwarf"; }

template <>
TerrainColorFractal<TerrainColorStarWhiteDwarf>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 3e9, 100.0); //why on Earth we need a feature size of 3,000,000 KM (2.2x the sun) I don't know, but we do... :)
	SetFracDef(1, height, 1e7, 100.0);
	//Original settings with correct feature size, does not seem to work anymore:
	//SetFracDef(0, height, 3e5, 10.0*m_fracmult);
	//SetFracDef(1, height, 1e5, 10.0*m_fracmult);
}

template <>
Vector3d TerrainColorFractal<TerrainColorStarWhiteDwarf>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	Vector3d col;
	n = ridged_octavenoise(GetFracDef(0), 0.8, p * p.x_);
	n += ridged_octavenoise(GetFracDef(1), 0.8, p);
	n += voronoiscam_octavenoise(GetFracDef(0), 0.8 * octavenoise(GetFracDef(1), 0.6, p), p);
	n *= n * n;
	if (n > 0.666) {
		n -= 0.666;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.8, .8, 1.0), Vector3d(1.0, 1.0, 1.0));
		return col;
	} else if (n > 0.333) {
		n -= 0.333;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.6, .8, .8), Vector3d(.8, .8, 1.0));
		return col;
	} else {
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.0, .0, .9), Vector3d(.6, .8, .8));
		return col;
	}
}



}



