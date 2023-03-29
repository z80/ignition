
#ifndef __MARCHING_CUBE_OBJECT_GD_H_
#define __MARCHING_CUBE_OBJECT_GD_H_

#include "core/reference.h"
#include "marching_volume_object.h"
#include "se3_ref.h"


namespace Ign
{

class MarchingVolumeObjectGd: public Reference
{
    GDCLASS( MarchingVolumeObjectGd, Reference );

protected:
    static void _bind_methods();

public:
    MarchingVolumeObjectGd();
    virtual ~MarchingVolumeObjectGd();

    void set_se3( const Ref<Se3Ref> & se3 );
    Ref<Se3Ref> get_se3() const;
    Vector3 at() const;

    void set_bounding_radius( real_t r );
    real_t get_bounding_radius() const;

public:
    MarchingVolumeObject * object;
};


}





#endif






