// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorGGJupiter>::GetColorFractalName() const { return "GGJupiter"; }

template <>
TerrainColorFractal<TerrainColorGGJupiter>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	// spots
	const Float height = m_maxHeightInMeters * 0.1;
	SetFracDef(0, height, 1e8, 1000.0);
	SetFracDef(1, height, 8e7, 1000.0);
	SetFracDef(2, height, 4e7, 1000.0);
	SetFracDef(3, height, 1e7, 100.0);
}

template <>
Vector3d TerrainColorFractal<TerrainColorGGJupiter>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n;
	const Float h = river_octavenoise(GetFracDef(0), 0.5 * m_entropy[0] + 0.25f,
						 Vector3d(piSimplex(Vector3d(p.x_ * 8, p.y_ * 32, p.z_ * 8)))) *
		.125;
	const Float equatorial_region_1 = billow_octavenoise(GetFracDef(0), 0.7, p) * p.y_ * p.x_;
	const Float equatorial_region_2 = octavenoise(GetFracDef(1), 0.8, p) * p.x_ * p.x_;
	Vector3d col;
	col = interpolate_color(equatorial_region_1, m_ggdarkColor[0], m_ggdarkColor[1]);
	col = interpolate_color(equatorial_region_2, col, Vector3d(.45, .3, .0));
	//top stripe
	if (p.y_ < 0.5 && p.y_ > 0.1) {
		for (float i = -1; i < 1; i += 0.6f) {
			Float temp = p.y_ - i;
			if (temp < .15 + h && temp > -.15 + h) {
				n = billow_octavenoise(GetFracDef(2), 0.7 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				n += 0.5 * octavenoise(GetFracDef(1), 0.6 * m_entropy[0], piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii, p.z_)) * p);
				n += ridged_octavenoise(GetFracDef(1), 0.6 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				//n += 0.5;
				n *= n;
				n = (n < 0.0 ? -n : n);
				n = (n > 1.0 ? 2.0 - n : n);
				if (n > 0.8) {
					n -= 0.8;
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[7]);
					return col;
				} else if (n > 0.6) {
					n -= 0.6;
					n *= 5.0;
					col = interpolate_color(n, m_gglightColor[4], col);
					return col;
				} else if (n > 0.4) {
					n -= 0.4;
					n *= 5.0;
					col = interpolate_color(n, Vector3d(.9, .89, .85), m_gglightColor[4]);
					return col;
				} else if (n > 0.2) {
					n -= 0.2;
					n *= 5.0;
					col = interpolate_color(n, m_ggdarkColor[2], Vector3d(.9, .89, .85));
					return col;
				} else {
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[2]);
					return col;
				}
			}
		} // bottom stripe
	} else if (p.y_ < -0.1 && p.y_ > -0.5) {
		for (float i = -1; i < 1; i += 0.6f) {
			Float temp = p.y_ - i;
			if (temp < .15 + h && temp > -.15 + h) {
				n = billow_octavenoise(GetFracDef(2), 0.6 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				n += 0.5 * octavenoise(GetFracDef(1), 0.7 * m_entropy[0], piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii, p.z_)) * p);
				n += ridged_octavenoise(GetFracDef(1), 0.6 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				//n += 0.5;
				//n *= n;
				n = (n < 0.0 ? -n : n);
				n = (n > 1.0 ? 2.0 - n : n);
				if (n > 0.8) {
					n -= 0.8;
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[7]);
					return col;
				} else if (n > 0.6) {
					n -= 0.6;
					n *= 5.0;
					col = interpolate_color(n, m_gglightColor[4], col);
					return col;
				} else if (n > 0.4) {
					n -= 0.4;
					n *= 5.0;
					col = interpolate_color(n, Vector3d(.9, .89, .85), m_gglightColor[4]);
					return col;
				} else if (n > 0.2) {
					n -= 0.2;
					n *= 5.0;
					col = interpolate_color(n, m_ggdarkColor[2], Vector3d(.9, .89, .85));
					return col;
				} else {
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[2]);
					return col;
				}
			}
		}
	} else { //small stripes
		for (float i = -1; i < 1; i += 0.3f) {
			Float temp = p.y_ - i;
			if (temp < .1 + h && temp > -.0 + h) {
				n = billow_octavenoise(GetFracDef(2), 0.6 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				n += 0.5 * octavenoise(GetFracDef(1), 0.6 * m_entropy[0], piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii, p.z_)) * p);
				n += ridged_octavenoise(GetFracDef(1), 0.7 * m_entropy[0],
					piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 0.3, p.z_)) * p);
				//n += 0.5;
				//n *= n;
				n = (n < 0.0 ? -n : n);
				n = (n > 1.0 ? 2.0 - n : n);
				if (n > 0.8) {
					n -= 0.8;
					n *= 5.0;
					col = interpolate_color(n, col, m_ggdarkColor[7]);
					return col;
				} else if (n > 0.6) {
					n -= 0.6;
					n *= 5.0;
					col = interpolate_color(n, m_gglightColor[4], col);
					return col;
				} else if (n > 0.4) {
					n -= 0.4;
					n *= 5.0;
					col = interpolate_color(n, Vector3d(.9, .89, .85), m_gglightColor[4]);
					return col;
				} else if (n > 0.2) {
					n -= 0.2;
					n *= 5.0;
					col = interpolate_color(n, m_ggdarkColor[2], Vector3d(.9, .89, .85));
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
	n = octavenoise(GetFracDef(1), 0.6 * m_entropy[0] + 0.25f, piSimplex(Vector3d(p.x_, p.y_ * m_planetEarthRadii * 3, p.z_)) * p);
	n *= n * n;
	n = (n < 0.0 ? -n : n);
	n = (n > 1.0 ? 2.0 - n : n);

	if (n > 0.5) {
		n -= 0.5;
		n *= 2.0;
		col = interpolate_color(n, col, m_gglightColor[2]);
		return col;
	} else {
		n *= 2.0;
		col = interpolate_color(n, Vector3d(.9, .89, .85), col);
		return col;
	}
}

}


