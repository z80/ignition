// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorTFGood>::GetColorFractalName() const { return "TFGood"; }

template <>
TerrainColorFractal<TerrainColorTFGood>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	m_surfaceEffects |= Terrain::EFFECT_WATER;
}

template <>
Vector3d TerrainColorFractal<TerrainColorTFGood>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height;
	const Float flatness = pow(p.DotProduct(norm), 8.0);
	Vector3d color_cliffs = m_rockColor[5];
	// ice on mountains and poles
	if (fabs(m_icyness * p.y) + m_icyness * n > 1) {
		return interpolate_color(flatness, color_cliffs, Vector3d(1, 1, 1));
	}

	Float equatorial_desert = (2.0 - m_icyness) * (-1.0 + 2.0 * octavenoise(12, 0.5, 2.0, (n * 2.0) * p)) *
		1.0 * (2.0 - m_icyness) * (1.0 - p.y * p.y);
	// This is for fake ocean depth by the coast.
	Float continents = octavenoise(GetFracDef(0), 0.7 * ridged_octavenoise(GetFracDef(8), 0.58, p), p) - m_sealevel * 0.6;

	Vector3d col;
	//we don't want water on the poles if there are ice-caps
	if (fabs(m_icyness * p.y) > 0.75) {
		col = interpolate_color(equatorial_desert, Vector3d(0.42, 0.46, 0), Vector3d(0.5, 0.3, 0));
		col = interpolate_color(flatness, col, Vector3d(1, 1, 1));
		return col;
	}
	// water
	if (n <= 0) {
		// Oooh, pretty coastal regions with shading based on underwater depth.
		n += continents - (GetFracDef(0).amplitude * m_sealevel * 0.49);
		n *= 10.0;
		n = (n > 0.3 ? 0.3 - (n * n * n - 0.027) : n);
		col = interpolate_color(equatorial_desert, Vector3d(0, 0, 0.15), Vector3d(0, 0, 0.25));
		col = interpolate_color(n, col, Vector3d(0, 0.8, 0.6));
		return col;
	}

	// More sensitive height detection for application of colours

	if (n > 0.5) {
		col = interpolate_color(equatorial_desert, m_rockColor[2], m_rockColor[4]);
		col = interpolate_color(n, col, m_darkrockColor[6]);
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > 0.25) {
		color_cliffs = m_darkrockColor[1];
		col = interpolate_color(equatorial_desert, m_darkrockColor[5], m_darkrockColor[7]);
		col = interpolate_color(n, col, m_rockColor[1]);
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > 0.05) {
		col = interpolate_color(equatorial_desert, m_darkrockColor[5], m_darkrockColor[7]);
		color_cliffs = col;
		col = interpolate_color(equatorial_desert, Vector3d(.45, .43, .2), Vector3d(.4, .43, .2));
		col = interpolate_color(n, col, Vector3d(-1.66, -2.3, -1.75));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > 0.01) {
		color_cliffs = Vector3d(0.2, 0.28, 0.2);
		col = interpolate_color(equatorial_desert, Vector3d(.15, .5, -.1), Vector3d(.2, .6, -.1));
		col = interpolate_color(n, col, Vector3d(5, -5, 5));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else if (n > 0.005) {
		color_cliffs = Vector3d(0.25, 0.28, 0.2);
		col = interpolate_color(equatorial_desert, Vector3d(.45, .6, 0), Vector3d(.5, .6, .0));
		col = interpolate_color(n, col, Vector3d(-10, -10, 0));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	} else {
		color_cliffs = Vector3d(0.3, 0.1, 0.0);
		col = interpolate_color(equatorial_desert, Vector3d(.35, .3, 0), Vector3d(.4, .3, .0));
		col = interpolate_color(n, col, Vector3d(0, 20, 0));
		col = interpolate_color(flatness, color_cliffs, col);
		return col;
	}
}
