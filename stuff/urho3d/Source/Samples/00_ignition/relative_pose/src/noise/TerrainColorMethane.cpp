// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorMethane>::GetColorFractalName() const { return "Methane"; }

template <>
TerrainColorFractal<TerrainColorMethane>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorMethane>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height;
	if (n <= 0)
		return Vector3d(.3, .0, .0);
	else
		return interpolate_color(n, Vector3d(.3, .2, .0), Vector3d(.6, .3, .0));
}

}



