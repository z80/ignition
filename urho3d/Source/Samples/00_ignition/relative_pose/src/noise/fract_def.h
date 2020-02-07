
#ifndef FRACDEF_H_INCLUDED
#define FRACDEF_H_INCLUDED

#include "data_types.h"

namespace Ign
{

struct fracdef_t {
	fracdef_t() :
		amplitude(0.0),
		frequency(0.0),
		lacunarity(0.0),
		octaves(0) {}
	Float amplitude;
	Float frequency;
	Float lacunarity;
	int octaves;
};

}

#endif // FRACDEF_H_INCLUDED



