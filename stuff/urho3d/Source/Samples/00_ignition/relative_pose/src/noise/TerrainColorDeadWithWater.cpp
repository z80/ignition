// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"
#include "pi_terrain_noise.h"

namespace Ign {

template <>
const char *TerrainColorFractal<TerrainColorDeadWithWater>::GetColorFractalName() const { return "DeadWithWater"; }

template <>
TerrainColorFractal<TerrainColorDeadWithWater>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
	m_surfaceEffects |= PiBodySource::EFFECT_WATER;
}

template <>
Vector3d TerrainColorFractal<TerrainColorDeadWithWater>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	Float n = m_invMaxHeight * height;
	if (n <= 0)
		return Vector3d(0.0, 0.0, 0.5);
	else
		return interpolate_color(n, Vector3d(.2, .2, .2), Vector3d(.6, .6, .6));
}

}
