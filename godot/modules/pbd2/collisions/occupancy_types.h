
#ifndef __OCCUPANCY_TYPES_H_
#define __OCCUPANCY_TYPES_H_

#include "data_types.h"
#include "vector3d.h"
#include "se3.h"


namespace Pbd
{

struct Vertex
{
    Vector3d at;
};

struct Plane
{
    Vector3d norm;
    Float    d;

    void init( const Vector3d & a, const Vector3d & b, const Vector3d & c );
    bool above( const Vector3d & r ) const;
    bool intersects( const Vector3d & a, const Vector3d & b, Vector3d & at ) const;
};

struct Face
{
    int inds[3];
    Vector3d verts[3];
    Plane    plane;
    Plane    planes[3];
    void init();
    void init_verts_and_planes()
    bool intersects( const Vector3d & r1, const Vector3d & r2, Vector3d & at ) const;
    bool intersects( const Face & f, Vector3d & at, Vector3d & depth ) const;
};

struct Cube
{
    // These are half sizes.
    Float    szx2, szy2, szz2;
    // Separate origin and center.
    // Transformation occurs with respect to the origin 
    // in order to make all cubes in octtree transform as a single rigid body.
    Vector3d origin, center;
    Vector3d ex, ey, ez;

    Quaterniond q;
    Vector3d    verts[8];
    Plane       planes[6];

    void init();
    Cube operator*( const SE3 & se3 ) const;
    bool intersects( const Vector3d & a, const Vector3d & b ) const;
    bool intersects( const Cube & c ) const;
};





}




#endif



