
#ifndef __VOLUME_SOURCE_H_
#define __VOLUME_SOURCE_H_

#include "marching_volume_object.h"

namespace Ign
{

class VolumeSource: public MarchingVolumeObject
{
public:
    VolumeSource();
    virtual ~VolumeSource();

    // This one computes value in global space.
    Float value_global( const Vector3d & at );

    // This one is computing in local space.
    virtual Float value( const Vector3d & at );

    void set_inverted( bool en );
    bool get_inverted() const;

	int material_global( const Vector3d & at, int * priority = nullptr );
	virtual int material( const Vector3d & at, int * priority = nullptr );

public:
    bool inverted;
	bool material_only;
};


}





#endif



