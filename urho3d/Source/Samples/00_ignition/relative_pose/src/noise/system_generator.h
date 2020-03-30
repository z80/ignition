
#ifndef __SYSTEM_GENERATOR_H_
#define __SYSTEM_GENERATOR_H_

#include "data_types.h"
#include "environment.h"
#include "pi_system.h"

using namespace Urho3D;

namespace Ign
{

class PiSystem;
class SphereDynamic;

class SystemGenerator: public Urho3D::Object
{
    URHO3D_OBJECT( SystemGenerator, Object )
public:
    static void RegisterObject( Context * context );

	SystemGenerator( Context * context );
	~SystemGenerator();

    void createBodies( Scene * scene );

	void generateSystem();
	void createBody( Scene * scene, RefFrame * parent, PiSystem & s, int bodyIndex );
    void applyBody( SphereDynamic * sd );
    SphereDynamic * homePlanet() const;

public:
    PiSystem s_;
    SharedPtr<SphereDynamic> homePlanet_; 
};

}



#endif





