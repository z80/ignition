// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_feature.h"
#include "pi_terrain_noise.h"

namespace Ign {
namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGSaturn2>::GetColorFractalName() const { return "GGSaturn2"; }

template <>
TerrainColorFractal<TerrainColorGGSaturn2>::TerrainColorFractal(const PiSourceDesc&body) :
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
Vector3d TerrainColorFractal<TerrainColorGGSaturn2>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = 0.2 * billow_octavenoise(GetFracDef(0), 0.8, p * p.y_ * p.y_);
	n += 0.5 * ridged_octavenoise(GetFracDef(1), 0.7, p * p.y_ * p.y_);
	n += 0.25 * octavenoise(GetFracDef(2), 0.7, p * p.y_ * p.y_);
	//spot
	n *= n * n * 0.5;
	n += billow_octavenoise(GetFracDef(0), 0.8, piSimplex(p * 3.142) * p) *
		megavolcano_function(GetFracDef(3), p);
	Vector3d col;
	//col = interpolate_color(octavenoise(GetFracDef(2), 0.7, piSimplex(p*3.142)*p), Vector3d(.05, .0, .0), Vector3d(.4,.0,.35));
	if (n > 1.0) {
		n -= 1.0; // n *= 5.0;
		col = interpolate_color(n, Vector3d(.25, .3, .4), Vector3d(.0, .2, .0));
	} else if (n > 0.8) {
		n -= 0.8;
		n *= 5.0;
		col = interpolate_color(n, Vector3d(.0, .0, .15), Vector3d(.25, .3, .4));
		return col;
	} else if (n > 0.6) {
		n -= 0.6;
		n *= 5.0;
		col = interpolate_color(n, Vector3d(.0, .0, .1), Vector3d(.0, .0, .15));
		return col;
	} else if (n > 0.4) {
		n -= 0.4;
		n *= 5.0;
		col = interpolate_color(n, Vector3d(.05, .0, .05), Vector3d(.0, .0, .1));
		return col;
	} else if (n > 0.2) {
		n -= 0.2;
		n *= 5.0;
		col = interpolate_color(n, Vector3d(.0, .0, .1), Vector3d(.05, .0, .05));
		return col;
	} else {
		n *= 5.0;
		col = interpolate_color(n, Vector3d(.0, .0, .0), Vector3d(.0, .0, .1));
		return col;
	}
	// never happens, just silencing a warning
	return col;
}
