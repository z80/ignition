
#ifndef __PBD_OCCUPANCY_TYPES_H_
#define __PBD_OCCUPANCY_TYPES_H_

#include "data_types.h"
#include "vector3d.h"
#include "pbd2_pose.h"


using namespace Ign;

namespace Pbd
{

bool common_perpendicular( const Vector3d & ra0, const Vector3d & ra1, const Vector3d & rb0, const Vector3d & rb1, Vector3d & ra, Vector3d & rb );


struct Plane
{
    Vector3d norm;
    Float    d;
    Vector3d r0;

    void init( const Vector3d & a, const Vector3d & b, const Vector3d & c );
    bool above( const Vector3d & r ) const;
    bool intersects( const Vector3d & a, const Vector3d & b, Vector3d & at ) const;
};

struct Face
{
    // Vertices in local ref. frame.
    Vector3d verts_0[3];
    // Vertex indices in the mesh.
    int inds[3];
    // Vertices in gloval ref. frame.
    Vector3d verts[3];
    // Face plane.
    Plane    plane;
    // Outward edge planes.
    Plane    planes[3];
    
    void init( const Vector3d & a, const Vector3d & b, const Vector3d & c );
    void init();
    void apply( const Pose & se3 );
    void init_planes();
        Float distance( const Vector3d & r ) const;
        Float distance( const Vector3d & r, Vector3d & displacement ) const;
        // Intersects with a ray.
        // And if intersects steps further "eps" along "a" and returns the next point.
        bool intersects_ray( const Vector3d & r0, const Vector3d & a, Float max_dist, Vector3d & next_r, Float eps=0.0001 );
    bool intersects( const Vector3d & r1, const Vector3d & r2, Vector3d & at ) const;
        bool intersects_eps( const Vector3d & r1, const Vector3d & r2, Float eps, Vector3d & at, bool & concerning ) const;
        bool intersects( const Face & f, Vector3d & at, Vector3d & depth ) const;
    // Actually, can be 0, 1 or 2 intersections.
    int intersects_all( const Face & f, Vector3d * at, Vector3d * depth ) const;

    // Colliding using common line.
    // Always either 0 or exactly 2 points.
        // Also return which triangle's edge it is "this" thiangle or "f" triangle.
        // And edge index.
        // The idea is that after all triangles are collided in the case of multiple collisions per edge ,
        // the smallest one is used.
    int intersects_2( const Face & f, Vector3d * at, Vector3d * depth, bool * this_edge, int * edge_index ) const;
};

struct Cube
{
    // Center before transform.
    Vector3d center_0;
    // These are half sizes.
    Float    szx2, szy2, szz2;
    Vector3d center;
    Vector3d ex, ey, ez;
    // AABB size.
    Float aabb2;

    Vector3d    verts[8];
    Plane       planes[6];

    void init( const Vector3d & c, Float x2, Float y2, Float z2 );
    void apply( const Pose & se3, bool only_axes=false );
    void init_verts_and_planes();
    // Intersects line
    bool intersects( const Vector3d & a, const Vector3d & b ) const;
    // Intersects another cube
    bool intersects( const Cube & c ) const;
    // Intersects face (triangle)
    bool intersects( const Face & f ) const;

    // Contains point inside
    bool contains( const Vector3d & at ) const;
};





}




#endif



