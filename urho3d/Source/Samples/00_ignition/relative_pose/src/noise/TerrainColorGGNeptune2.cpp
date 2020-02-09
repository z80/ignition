// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGNeptune2>::GetColorFractalName() const { return "GGNeptune2"; }

template <>
TerrainColorFractal<TerrainColorGGNeptune2>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	// spots
	const Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 2e8, 1000.0);
	SetFracDef(1, height, 9e7, 1000.0);
	SetFracDef(2, height, 6e7, 1000.0);
	SetFracDef(3, height, 1e8, 100.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorGGNeptune2>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	const Float h = billow_octavenoise(GetFracDef(0), 0.5 * m_entropy[0] + 0.25f,
						 Vector3d(noise(Vector3d(p.x * 8, p.y * 32, p.z * 8)))) *
		.125;
	const Float equatorial_region_1 = billow_octavenoise(GetFracDef(0), 0.54, p) * p.y * p.x;
	const Float equatorial_region_2 = octavenoise(GetFracDef(1), 0.58, p) * p.x * p.x;
	Vector3d col;
	col = interpolate_color(equatorial_region_1, Vector3d(.01, .01, .1), m_ggdarkColor[0]);
	col = interpolate_color(equatorial_region_2, col, Vector3d(0, 0, .2));
	//stripes
	if (p.y < 0.5 && p.y > -0.5) {
		for (float i = -1; i < 1; i += 0.6f) {
			Float temp = p.y - i;
			if (temp < .07 + h && temp > -.07 + h) {
				n = 2.0 * billow_octavenoise(GetFracDef(2), 0.5 * m_entropy[0], noise(Vector3d(p.x, p.y * m_planetEarthRadii * 0.3, p.z)) * p);
				n += 0.8 * octavenoise(GetFracDef(1), 0.5 * m_entropy[0], noise(Vector3d(p.x, p.y * m_planetEarthRadii, p.z)) * p);
				n += 0.5 * billow_octavenoise(GetFracDef(3), 0.6, p);
				n *= n;
				n = (n < 0.0 ? -n : n);
				n = (n > 1.0 ? 2.0 - n : n);
				if (n > 0.8) {
					n -= 0.8;
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[2]);
					return col;
				} else if (n > 0.6) {
					n -= 0.6;
					n *= 5.0;
					col = interpolate_color(n, Vector3d(.03, .03, .15), col);
					return col;
				} else if (n > 0.4) {
					n -= 0.4;
					n *= 5.0;
					col = interpolate_color(n, Vector3d(.0, .0, .05), Vector3d(.03, .03, .15));
					return col;
				} else if (n > 0.2) {
					n -= 0.2;
					n *= 5.0;
					col = interpolate_color(n, m_ggdarkColor[2], Vector3d(.0, .0, .05));
					return col;
				} else {
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[2]);
					return col;
				}
			}
		}
	}
	//if is not a stripe.
	n = octavenoise(GetFracDef(1), 0.5 * m_entropy[0] + 0.25f, noise(Vector3d(p.x * 0.2, p.y * m_planetEarthRadii * 10, p.z)) * p);
	//n += 0.5;
	//n += octavenoise(GetFracDef(0), 0.6*m_entropy[0], 3.142*p.z*p.z);
	n *= n * n * n;
	n = (n < 0.0 ? -n : n);
	n = (n > 1.0 ? 2.0 - n : n);

	if (n > 0.5) {
		n -= 0.5;
		n *= 2.0;
		col = interpolate_color(n, col, m_ggdarkColor[2]);
		return col;
	} else {
		n *= 2.0;
		col = interpolate_color(n, Vector3d(.0, .0, .0), col);
		return col;
	}
}
