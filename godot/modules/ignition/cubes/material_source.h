
#ifndef __MATERIAL_SOURCE_H_
#define __MATERIAL_SOURCE_H_

#include "marching_volume_object.h"

namespace Ign
{

class MaterialSource: public MarchingVolumeObject
{
public:
    MaterialSource();
    virtual ~MaterialSource();

    // This one computes value in global space.
    virtual int material( const Vector3d & at ) const;

	void set_weak( bool weak );
	bool get_weak() const;

public:
	bool weak;
};


}




#endif






