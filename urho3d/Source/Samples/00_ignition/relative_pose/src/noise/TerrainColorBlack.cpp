// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"

static const vector3d svBlack(0.0, 0.0, 0.0);

template <>
const char *TerrainColorFractal<TerrainColorBlack>::GetColorFractalName() const { return "Solid"; }

template <>
TerrainColorFractal<TerrainColorBlack>::TerrainColorFractal(const PiSourceDescs&body) :
	PiBodySource(body)
{
}

template <>
vector3d TerrainColorFractal<TerrainColorBlack>::GetColor(const vector3d &p, double height, const vector3d &norm) const
{
	return svBlack;
}
