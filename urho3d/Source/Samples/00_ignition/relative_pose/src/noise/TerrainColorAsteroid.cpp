// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorAsteroid>::GetColorFractalName() const { return "Asteroid"; }

template <>
TerrainColorFractal<TerrainColorAsteroid>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorAsteroid>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height / 2;

	if (n <= 0.02) {
		const Float flatness = pow(p.DotProduct(norm), 6.0);
		const Vector3d color_cliffs = m_rockColor[1];

		Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
			1.0 * (2.0) * (1.0 - p.y_ * p.y_);

		Vector3d col;
		col = interpolate_color(equatorial_desert, m_rockColor[0], m_greyrockColor[3]);
		col = interpolate_color(n, col, Vector3d(1.5, 1.35, 1.3));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else {
		const Float flatness = pow(p.DotProduct(norm), 6.0);
		const Vector3d color_cliffs = m_greyrockColor[1];

		Float equatorial_desert = (2.0) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
			1.0 * (2.0) * (1.0 - p.y_ * p.y_);

		Vector3d col;
		col = interpolate_color(equatorial_desert, m_greyrockColor[0], m_greyrockColor[2]);
		col = interpolate_color(n, col, m_rockColor[3]);
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	}
}
