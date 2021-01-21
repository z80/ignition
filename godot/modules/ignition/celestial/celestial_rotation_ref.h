
#ifndef __CELESTIAL_ROTATION_REF_H_
#define __CELESTIAL_ROTATION_REF_H_

#include "core/reference.h"
#include "celestial_rotation.h"

namespace Ign
{

class CelestialRotationRef: public Reference
{
    GDCLASS( CelestialRotationRef, Reference );
    OBJ_CATEGORY("Ignition");

protected:
    static void _bind_methods();

public:
    CelestialRotationRef();
    ~CelestialRotationRef();

    void init( const Vector3 & up, real_t period_hrs );
    void process_rf( real_t dt, Node * rf );

public:
    CelestialRotation celestial_rotation;
};



}






#endif











