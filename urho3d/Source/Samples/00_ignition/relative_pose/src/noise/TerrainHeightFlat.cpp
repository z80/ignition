// Copyright © 2008-2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "pi_source.h"

template <>
const char *TerrainHeightFractal<TerrainHeightFlat>::GetHeightFractalName() const { return "Flat"; }

template <>
TerrainHeightFractal<TerrainHeightFlat>::TerrainHeightFractal(const PiSourceDesc&body) :
	PiBodySource(body)
{
}

template <>
Float TerrainHeightFractal<TerrainHeightFlat>::GetHeight(const Vector3d &p) const
{
	return 0.0;
}
