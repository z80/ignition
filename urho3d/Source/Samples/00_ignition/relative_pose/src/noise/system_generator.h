
#ifndef __SYSTEM_GENERATOR_H_
#define __SYSTEM_GENERATOR_H_

#include "data_types.h"
#include "environment.h"

using namespace Urho3D;

namespace Ign
{

class PiSystem;

class SystemGenerator
{
public:
	SystemGenerator();
	~SystemGenerator();

	void generate( Scene * scene );

	void generateSystem( PiSystem & s );
	void createBody( Scene * scene, RefFrame * parent, PiSystem & s, int bodyIndex );
};

}



#endif





