// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"

namespace Ign
{

static const Vector3d svWhite(1.0, 1.0, 1.0);

template <>
const char *TerrainColorFractal<TerrainColorWhite>::GetColorFractalName() const { return "Solid"; }

template <>
TerrainColorFractal<TerrainColorWhite>::TerrainColorFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Vector3d TerrainColorFractal<TerrainColorWhite>::GetColor(const Vector3d &p, Float height, const Vector3d &norm) const
{
	return svWhite;
}



}

