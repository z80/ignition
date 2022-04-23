
#ifndef __VOLUME_SOURCE_H_
#define __VOLUME_SOURCE_H_



#include "data_types.h"
#include "vector3d.h"
#include "se3.h"

namespace Ign
{

class VolumeSource
{
public:
    VolumeSource();
    virtual ~VolumeSource();

	// This one computes value in global space.
	Float value_global( const Vector3d & at ) const;

	// This one is computing in local space.
	virtual Float value( const Vector3d & at ) const;

    void set_se3( const SE3 & se3 );
    const SE3 & get_se3() const;

    void set_inverted( bool en );
    bool get_inverted() const;

    virtual Float max_node_size() const;
    virtual Float min_node_size() const;
    virtual Float max_node_size_at( const Vector3d & at ) const;

    SE3  se3;
	SE3  se3_inverted;
    bool inverted;
};


}





#endif



