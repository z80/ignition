
#include "marching_volume_object.h"
#include "cube_tree_node.h"

namespace Ign
{

MarchingVolumeObject::MarchingVolumeObject()
{
    bounding_radius = 1.0;
}

MarchingVolumeObject::~MarchingVolumeObject()
{
}

bool MarchingVolumeObject::inside( const CubeTreeNode * node ) const
{
    const Vector3d at = se3.r_;
    const Vector3d bottom( at.x_ - bounding_radius, at.y_ - bounding_radius, at.z_ - bounding_radius );
    const Vector3d & corner_max = node->corner_max;
    if ( (corner_max.x_ < bottom.x_) || (corner_max.y_ < bottom.y_) || (corner_max.z_ < bottom.z_) )
        return false;

    const Vector3d top( at.x_ + bounding_radius, at.y_ + bounding_radius, at.z_ + bounding_radius );
    const Vector3d & corner_min = node->corner_min;
    if ( (corner_min.x_ > top.x_) || (corner_min.y_ > top.y_) || (corner_min.z_ > top.z_) )
        return false;

    return true;
}

void MarchingVolumeObject::set_se3( const SE3 & se3 )
{
    this->se3 = se3;
    this->se3_inverted = se3.inverse();
}

const SE3 & MarchingVolumeObject::get_se3() const
{
    return se3;
}

Vector3d MarchingVolumeObject::at() const
{
    const Vector3d ret = se3.r_;
    return ret;
}

void MarchingVolumeObject::set_bounding_radius( Float r )
{
    bounding_radius = r;
}

Float MarchingVolumeObject::get_bounding_radius() const
{
    return bounding_radius;
}

Float MarchingVolumeObject::max_node_size() const
{
    return 1.0;
}

Float MarchingVolumeObject::min_node_size() const
{
    return 1.0;
}

Float MarchingVolumeObject::max_node_size_local( const Vector3d & at ) const
{
    return 1.0;
}

Float MarchingVolumeObject::min_node_size_local( const Vector3d & at ) const
{
	return 1.0;
}

Float MarchingVolumeObject::max_node_size_at( const Vector3d & at ) const
{
    const Vector3d l_at = local_at( at );
    const Float ret = max_node_size_local( l_at );
    return ret;
}

Float MarchingVolumeObject::min_node_size_at( const Vector3d & at ) const
{
	const Vector3d l_at = local_at( at );
	const Float ret = min_node_size_local( l_at );
	return ret;
}

Vector3d MarchingVolumeObject::local_at( const Vector3d & global_at ) const
{
    const Vector3d ret = se3_inverted.q_ * global_at + se3_inverted.r_;
    return ret;
}



}










