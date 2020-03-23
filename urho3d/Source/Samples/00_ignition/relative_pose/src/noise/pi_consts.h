// Copyright © 2008-2020 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __PI_CONSTS_H_
#define __PI_CONSTS_H_

#include "data_types.h"


namespace Ign
{

static const Float PHYSICS_HZ = 60.0;

static const Float MAX_LANDING_SPEED = 30.0; // m/sec
static const Float LIGHT_SPEED = 3e8; // m/sec

static const uint32_t UNIVERSE_SEED = 0xabcd1234;

static const Float EARTH_RADIUS = 6378135.0 / 1.0e3; // m
static const Float EARTH_MASS = 5.9742e24 / 1.0e6; // Kg
static const Float SOL_RADIUS = 6.955e8 / 1.0e3; // m
static const Float SOL_MASS = 1.98892e30 / 1.0e6; // Kg

static const Float AU = 149598000000.0 / 1.0e7; // m
static const Float G = 6.67428e-11;

static const Float EARTH_ATMOSPHERE_SURFACE_DENSITY = 1.225;
static const Float GAS_CONSTANT_R = 8.3144621;

#ifdef M_PI
	#undef M_PI
#endif
static const Float M_PI = 3.1415926535897932384636433832795;

}


#endif 
