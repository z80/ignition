
#ifndef __MARCHING_VOLUME_OBJECT_H_
#define __MARCHING_VOLUME_OBJECT_H_

#include "data_types.h"
#include "cube_types.h"
#include "se3.h"


namespace Ign
{

class CubeTreeNode;

class MarchingVolumeObject
{
public:
    MarchingVolumeObject();
    virtual ~MarchingVolumeObject();

    virtual bool inside( const CubeTreeNode * node ) const;

    void set_se3( const SE3 & se3 );
    const SE3 & get_se3() const;
    Vector3d at() const;

    void set_bounding_radius( Float r );
    Float get_bounding_radius() const;

    virtual Float max_node_size() const;
    virtual Float min_node_size() const;
    virtual Float max_node_size_local( const Vector3d & at );
	virtual Float min_node_size_local( const Vector3d & at );
	Float max_node_size_at( const Vector3d & at );
	Float min_node_size_at( const Vector3d & at );

    Vector3d local_at( const Vector3d & global_at ) const;

    SE3   se3;
    SE3   se3_inverted;
    Float bounding_radius;
};




}





#endif





