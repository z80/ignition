// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorDesert>::GetColorFractalName() const { return "Desert"; }

template <>
TerrainColorFractal<TerrainColorDesert>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorDesert>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height / 2;
	const Float flatness = pow(p.DotProduct(norm), 6.0);
	const Vector3d color_cliffs = m_rockColor[1];
	// Ice has been left as is so the occasional desert world will have polar ice-caps like mars
	if (fabs(m_icyness * p.y_) + m_icyness * n > 1) {
		return interpolate_color(flatness, color_cliffs, Vector3d(1, 1, 1));
	}
	Float equatorial_desert = (2.0 - m_icyness) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
		1.0 * (2.0 - m_icyness) * (1.0 - p.y_ * p.y_);
	Vector3d col;
	if (n > .4) {
		n = n * n;
		col = interpolate_color(equatorial_desert, Vector3d(.8, .75, .5), Vector3d(.52, .5, .3));
		col = interpolate_color(n, col, Vector3d(.1, .0, .0));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > .3) {
		n = n * n;
		col = interpolate_color(equatorial_desert, Vector3d(.81, .68, .3), Vector3d(.85, .7, 0));
		col = interpolate_color(n, col, Vector3d(-1.2, -.84, .35));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > .2) {
		col = interpolate_color(equatorial_desert, Vector3d(-0.4, -0.47, -0.6), Vector3d(-.6, -.7, -2));
		col = interpolate_color(n, col, Vector3d(4, 3.95, 3.94));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else {
		col = interpolate_color(equatorial_desert, Vector3d(.78, .73, .68), Vector3d(.8, .77, .5));
		col = interpolate_color(n, col, Vector3d(-2.0, -2.3, -2.4));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	}
}

}
