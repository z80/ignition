// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"

static const vector3d svWhite(1.0, 1.0, 1.0);

template <>
const char *TerrainColorFractal<TerrainColorWhite>::GetColorFractalName() const { return "Solid"; }

template <>
TerrainColorFractal<TerrainColorWhite>::TerrainColorFractal(const PiSourceDescs&body) :
	Terrain(body)
{
}

template <>
vector3d TerrainColorFractal<TerrainColorWhite>::GetColor(const vector3d &p, double height, const vector3d &norm) const
{
	return svWhite;
}
