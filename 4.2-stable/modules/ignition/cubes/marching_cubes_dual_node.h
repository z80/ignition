
#ifndef __MARCHING_CUBES_DUAL_NODE_H_
#define __MARCHING_CUBES_DUAL_NODE_H_



#include "data_types.h"
#include "vector3d.h"
#include "core/templates/vector.h"
//#include "core/math/face3.h"
#include "core/math/aabb.h"

#include "se3.h"
#include "cube_hash.h"


#include <vector>
#include <set>
#include <map>

#include "cube_types.h"


namespace Ign
{

class MarchingCubesDual;
class VolumeSource;
class DistanceScalerBase;
class VolumeNodeSizeStrategy;

class MarchingCubesDualNode
{
public:
    MarchingCubesDualNode();
    ~MarchingCubesDualNode();
    MarchingCubesDualNode( const MarchingCubesDualNode & inst );
    const MarchingCubesDualNode & operator=( const MarchingCubesDualNode & inst );

	void query_nodes( const MarchingCubesDualNode & node, int sz, std::vector<int> & ret );
	void query_faces( MarchingCubesDual * tree, const MarchingCubesDualNode & node, std::set<int> & face_inds );

	bool intersect_with_segment( MarchingCubesDual * tree, const Vector3d & start, const Vector3d & end, Vector3d & at, Vector3d & norm );
	bool intersect_with_ray( MarchingCubesDual * tree, const Vector3d & start, const Vector3d & dir, Vector3d & at, Vector3d & norm );

    bool has_children() const;
    bool subdivide( MarchingCubesDual * tree, VolumeSource * source, VolumeNodeSizeStrategy * strategy, int level );
	void compute_hashes();

    // Initialize vertices and planes.
    void init_aabb( MarchingCubesDual * tree );

	String hierarchy_path() const;

	bool intersects( const MarchingCubesDualNode & other ) const;

	bool contains_point( const MarchingCubesDual * tree, const Vector3d & point ) const;
	Vector3d center_vector( const MarchingCubesDual * tree ) const;
	Float    node_size( const MarchingCubesDual * tree ) const;
	//SE3      se3_in_point( MarchingCubesDual * tree, const Vector3d & at, bool in_source ) const;
	//SE3      asset_se3( MarchingCubesDual * tree, const SE3 & asset_at, bool asset_in_source, bool result_in_source, const DistanceScalerBase * scaler ) const;

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



	// These functions for locating surrounding nodes.
	const MarchingCubesDualNode create_adjacent_node( int dx, int dy, int dz ) const;

public:
    int self_index;
    int index_in_parent;

    // Parent node.
    MarchingCubesDualNode * parent_node;
    // Child indices in Octtree list.
    MarchingCubesDualNode * child_nodes[8];

    Integer   size; // Size.
    VectorInt at;

    // Store these for convenience.
    VectorInt vertices_int[8];
    Vector3d  vertices[8];
    Float     values[8];

	// Hash for setting assets (in future).
	CubeHash hash;

	// Faces.
	int face_base_index;
	int faces_qty;

	// AABB for computing intersections.
	Aabb aabb;
};

}



#endif





