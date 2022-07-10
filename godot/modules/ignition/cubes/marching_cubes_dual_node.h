
#ifndef __MARCHING_CUBES_DUAL_NODE_H_
#define __MARCHING_CUBES_DUAL_NODE_H_



#include "data_types.h"
#include "vector3d.h"
#include "core/vector.h"
#include "core/math/face3.h"

#include "se3.h"


#include <vector>
#include <set>
#include <map>

#include "cube_types.h"


namespace Ign
{

class MarchingCubesDual;
class VolumeSource;
class DistanceScalerBase;

class MarchingCubesDualNode
{
public:
    MarchingCubesDualNode();
    ~MarchingCubesDualNode();
    MarchingCubesDualNode( const MarchingCubesDualNode & inst );
    const MarchingCubesDualNode & operator=( const MarchingCubesDualNode & inst );

    bool has_children() const;
    bool subdivide( MarchingCubesDual * tree, VolumeSource * source, const DistanceScalerBase * scaler );

    // Initialize vertices and planes.
    void init( MarchingCubesDual * tree );

    const VectorInt center() const;

    bool has_surface( Float iso_level ) const;

    bool at_left( const MarchingCubesDualNode * root ) const;
    bool at_right( const MarchingCubesDualNode * root ) const;
    bool at_bottom( const MarchingCubesDualNode * root ) const;
    bool at_top( const MarchingCubesDualNode * root ) const;
    bool at_back( const MarchingCubesDualNode * root ) const;
    bool at_front( const MarchingCubesDualNode * root ) const;

    inline const VectorInt center_back() const
    {
            return VectorInt( at.x + size/2, at.y + size/2, at.z );
    }

    inline const VectorInt center_front() const
    {
            return VectorInt( at.x + size/2, at.y + size/2, at.z + size );
    }

    inline const VectorInt center_left() const
    {
            return VectorInt( at.x, at.y + size/2, at.z + size/2 );
    }

    inline const VectorInt center_right(void) const
    {
            return VectorInt( at.x + size, at.y + size/2, at.z + size/2 );
    }

    inline const VectorInt center_top(void) const
    {
            return VectorInt( at.x + size/2, at.y + size, at.z + size/2 );
    }

    inline const VectorInt center_bottom(void) const
    {
            return VectorInt( at.x +size/2, at.y, at.z + size/2);
    }

    inline const VectorInt center_back_top(void) const
    {
            return VectorInt( at.x + size/2, at.y + size, at.z );
    }

    inline const VectorInt center_back_bottom(void) const
    {
            return VectorInt( at.x + size/2, at.y, at.z );
    }

    inline const VectorInt center_front_top(void) const
    {
            return VectorInt( at.x + size/2, at.y + size, at.z + size );
    }

    inline const VectorInt center_front_bottom(void) const
    {
            return VectorInt( at.x + size/2, at.y, at.z + size );
    }

    inline const VectorInt center_left_top(void) const
    {
            return VectorInt( at.x, at.y + size, at.z + size/2 );
    }

    inline const VectorInt center_left_bottom(void) const
    {
            return VectorInt( at.x, at.y, at.z + size/2 );
    }

    inline const VectorInt center_right_top(void) const
    {
            return VectorInt( at.x + size, at.y + size, at.z + size/2 );
    }

    inline const VectorInt center_right_bottom(void) const
    {
            return VectorInt( at.x + size, at.y, at.z + size/2 );
    }

    inline const VectorInt center_back_left(void) const
    {
            return VectorInt( at.x, at.y + size/2, at.z );
    }

    inline const VectorInt center_front_left(void) const
    {
            return VectorInt( at.x, at.y + size/2, at.z + size );
    }

    inline const VectorInt center_back_right(void) const
    {
            return VectorInt( at.x + size, at.y + size/2, at.z );
    }

    inline const VectorInt center_front_right(void) const
    {
            return VectorInt( at.x + size, at.y + size/2, at.z + size );
    }

    inline const VectorInt corner_0(void) const
    {
            return VectorInt( at.x, at.y, at.z );
    }

    inline const VectorInt corner_1(void) const
    {
            return VectorInt( at.x+size, at.y, at.z );
    }

    inline const VectorInt corner_2(void) const
    {
            return VectorInt( at.x+size, at.y, at.z+size );
    }

    inline const VectorInt corner_3(void) const
    {
            return VectorInt( at.x, at.y, at.z+size );
    }

    inline const VectorInt corner_4(void) const
    {
            return VectorInt( at.x, at.y+size, at.z );
    }

    inline const VectorInt corner_5(void) const
    {
            return VectorInt( at.x+size, at.y+size, at.z );
    }

    inline const VectorInt corner_6(void) const
    {
            return VectorInt( at.x+size, at.y+size, at.z+size );
    }

    inline const VectorInt corner_7(void) const
    {
            return VectorInt( at.x, at.y+size, at.z+size );
    }


public:
    int self_index;
    int index_in_parent;

    // Parent node.
    MarchingCubesDualNode * parent_node;
    // Child indices in Octtree list.
    MarchingCubesDualNode * child_nodes[8];

    int       size; // Size.
    VectorInt at;

    // Store these for convenience.
    VectorInt vertices_int[8];
    Vector3d  vertices_scaled[8];
    Vector3d  vertices[8];
    Float     values[8];
};

}



#endif





