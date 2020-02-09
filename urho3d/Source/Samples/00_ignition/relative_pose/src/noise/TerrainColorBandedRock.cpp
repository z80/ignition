// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorBandedRock>::GetColorFractalName() const { return "BandedRock"; }

template <>
TerrainColorFractal<TerrainColorBandedRock>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorBandedRock>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	const Float flatness = pow(p.Dot(norm), 6.0);
	Float n = fabs(noise(Vector3d(height * 10000.0, 0.0, 0.0)));
	Vector3d col = interpolate_color(n, m_rockColor[0], m_rockColor[1]);
	return interpolate_color(flatness, col, m_rockColor[2]);
}
