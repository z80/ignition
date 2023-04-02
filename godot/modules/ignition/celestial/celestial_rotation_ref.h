
#ifndef __CELESTIAL_ROTATION_REF_H_
#define __CELESTIAL_ROTATION_REF_H_

#include "core/object/ref_counted.h"
#include "scene/main/node.h"
#include "celestial_rotation.h"

namespace Ign
{

class CelestialRotationRef: public RefCounted
{
    GDCLASS( CelestialRotationRef, RefCounted );
    //OBJ_CATEGORY("Ignition");

protected:
    static void _bind_methods();

public:
    CelestialRotationRef();
    ~CelestialRotationRef();

    void init( const Vector3 & up, real_t period_hrs );
    void process_rf( real_t dt, Node * rf );

    Dictionary serialize() const;
    bool deserialize( const Dictionary & data );

public:
    CelestialRotation celestial_rotation;
};



}






#endif











