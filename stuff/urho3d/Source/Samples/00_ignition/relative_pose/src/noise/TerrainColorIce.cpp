// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorIce>::GetColorFractalName() const { return "Ice"; }

template <>
TerrainColorFractal<TerrainColorIce>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorIce>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height;

	if (n <= 0.0) return Vector3d(0.96, 0.96, 0.96);

	const Float flatness = pow(p.DotProduct(norm), 24.0);
	Float equatorial_desert = (2.0 - m_icyness) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
		1.0 * (2.0 - m_icyness) * (1.0 - p.y_ * p.y_);
	Float equatorial_region_1 = billow_octavenoise(GetFracDef(0), 0.5, p) * p.y_ * p.y_;
	Float equatorial_region_2 = ridged_octavenoise(GetFracDef(5), 0.5, p) * p.x_ * p.x_;
	// cliff colours
	Vector3d color_cliffs;
	// adds some variation
	color_cliffs = interpolate_color(equatorial_region_1, m_rockColor[3], m_rockColor[0]);
	color_cliffs = interpolate_color(equatorial_region_2, color_cliffs, m_rockColor[2]);
	// main colours
	Vector3d col;
	// start by interpolating between noise values for variation
	col = interpolate_color(equatorial_region_1, m_darkrockColor[0], Vector3d(1, 1, 1));
	col = interpolate_color(equatorial_region_2, m_darkrockColor[1], col);
	col = interpolate_color(equatorial_desert, col, Vector3d(.96, .95, .94));
	// scale by different colours depending on height for more variation
	if (n > .666) {
		n -= 0.666;
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.96, .95, .94), col);
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > 0.333) {
		n -= 0.333;
		n *= 3.0;
		col = interpolate_color(n, col, Vector3d(.96, .95, .94));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else {
		n *= 3.0;
		col = interpolate_color(n, Vector3d(.96, .95, .94), col);
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	}
}

}



