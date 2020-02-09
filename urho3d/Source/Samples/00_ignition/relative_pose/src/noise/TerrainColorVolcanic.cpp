// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorVolcanic>::GetColorFractalName() const { return "Volcanic"; }

template <>
TerrainColorFractal<TerrainColorVolcanic>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	// 50 percent chance of there being exposed lava
	if (m_rand.Int32(100) > 50)
		m_surfaceEffects |= Terrain::EFFECT_LAVA;
}

template <>
Vector3d TerrainColorFractal<TerrainColorVolcanic>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height;
	const Float flatness = pow(p.DotProduct(norm), 6.0);
	const Vector3d color_cliffs = m_rockColor[2];
	Float equatorial_desert = (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
		1.0 * (1.0 - p.y * p.y);

	Vector3d col;

	if (n > 0.4) {
		col = interpolate_color(equatorial_desert, Vector3d(.3, .2, 0), Vector3d(.3, .1, .0));
		col = interpolate_color(n, col, Vector3d(.1, .0, .0));
		col = interpolate_color(flatness, color_cliffs, col);
	} else if (n > 0.2) {
		col = interpolate_color(equatorial_desert, Vector3d(1.2, 1, 0), Vector3d(.9, .3, .0));
		col = interpolate_color(n, col, Vector3d(-1.1, -1, .0));
		col = interpolate_color(flatness, color_cliffs, col);
	} else if (n > 0.1) {
		col = interpolate_color(equatorial_desert, Vector3d(.2, .1, 0), Vector3d(.1, .05, .0));
		col = interpolate_color(n, col, Vector3d(2.5, 2, .0));
		col = interpolate_color(flatness, color_cliffs, col);
	} else {
		col = interpolate_color(equatorial_desert, Vector3d(.75, .6, 0), Vector3d(.75, .2, .0));
		col = interpolate_color(n, col, Vector3d(-2, -2.2, .0));
		col = interpolate_color(flatness, color_cliffs, col);
	}
	return col;
}
