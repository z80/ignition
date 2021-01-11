
#ifndef __PDB_OCCUPANCY_TESTS_REF_H_
#define __PDB_OCCUPANCY_TESTS_REF_H_

#include "core/reference.h"
#include "occupancy_types.h"

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
    bool cubes_intersect() const;

    void set_face_b( const Vector3 & a, const Vector3 & b, const Vector3 & c );
    void apply_to_face_b( const Transform & t );
    
    bool intersect_faces();
    Vector3 face_intersection_point() const;
    Vector3 face_depth_point() const;



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
    Vector3d face_face_intersection_point_, face_face_intersection_depth_;
};



}

#endif




