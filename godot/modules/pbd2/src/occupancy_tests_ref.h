
#ifndef __PDB_OCCUPANCY_TESTS_REF_H_
#define __PDB_OCCUPANCY_TESTS_REF_H_

#include "core/reference.h"
#include "occupancy_types.h"
#include "pbd2_narrow_tree.h"

namespace Pbd
{


class OccupancyTestsRef: public Reference
{
    GDCLASS( OccupancyTestsRef, Reference );
protected:
    static void _bind_methods();

public:
    OccupancyTestsRef();
    ~OccupancyTestsRef();

    void set_ray_a( const Vector3 & a, const Vector3 & b );
    void set_ray_b( const Vector3 & a, const Vector3 & b );
    bool intersect_rays();
    Vector3 common_perpendicualr_a() const;
    Vector3 common_perpendicualr_b() const;

    void set_cube( const Vector3 & c, real_t sz_x, real_t sz_y, real_t sz_z );
    void apply_to_cube( const Transform & t );

    void set_face( const Vector3 & a, const Vector3 & b, const Vector3 & c );
    void apply_to_face( const Transform & t );

    bool intersect_face_with_cube() const;

    void set_cube_b( const Vector3 & c, real_t sz_x, real_t sz_y, real_t sz_z );
    void apply_to_cube_b( const Transform & t );
    bool intersect_cubes() const;

    void set_face_b( const Vector3 & a, const Vector3 & b, const Vector3 & c );
    void apply_to_face_b( const Transform & t );
    
    int intersect_faces();
    Vector3 face_intersection_point( int ind ) const;
    Vector3 face_depth_point( int ind ) const;



    Vector3d ray_a0_, ray_a1_;
    Vector3d ray_b0_, ray_b1_;
    Vector3d common_perp_a_, common_perp_b_;

    Cube ccc;
    Cube cube_;

    Face face_;
    bool face_intersects_cube_;

    Cube cube_b_;
    bool cubes_intersect_;

    Face face_b_;
    Vector3d face_face_intersection_point_[3], face_face_intersection_depth_[3];


    NarrowTree tree_;

    void tree_apply( const Transform & t );
    void tree_clear();
    void tree_add_triangle( const Vector3 & a, const Vector3 & b, const Vector3 & c );
    void tree_set_level( int level );
    void tree_subdivide();
    Array tree_depth( const Vector3 & at );
    Vector3 tree_probe_depth( const Vector3 & at );

    PoolVector3Array lines_sdf_nodes() const;
    PoolVector3Array lines_surface_pts() const;
    PoolVector3Array lines_pts_nodes() const;
    PoolVector3Array lines_aligned_nodes() const;

};



}

#endif




