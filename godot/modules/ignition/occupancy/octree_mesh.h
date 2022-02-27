
#ifndef __OCTREE_MESH_H_
#define __OCTREE_MESH_H_

#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"
#include "octree_mesh_node.h"

#include "se3.h"

namespace Ign
{

/**
The idea is to use this for quick search if ray intersects a mesh (a set of meshes).
I'm going to use it for air drag computation. If forward or backward pointing face
forward direction intersects with any other meshes, exclude it from forces computation and
assume it is occluded by something in front.
But of course, don't intersect with own mesh. As I need all faces to contribute.
**/

class OctreeMesh
{
public:
    struct FaceProperties
    {
        // Face area.
        real_t  area;
        // Normal vector.
        Vector3 normal;
        // Median point position.
        Vector3 position;
    };


    OctreeMesh();
    ~OctreeMesh();

	void set_max_depth( int val );
	int get_max_depth() const;
	void set_min_faces( int val );
	int get_min_faces() const;

    // Manipulating filling up the occupancy grid.
    void clear();
    void append( const Transform & t, const Ref<Mesh> mesh );
    void subdivide();

    void set_origin( const Vector3 & at );
    const Vector3 & get_origin() const;
    void set_quat( const Quat & q );
    const Quat & get_quat() const;
    void set_se3( const SE3 & se3 );
    SE3  get_se3() const;

    real_t size2() const;

    // Intersects with infinite ray.
    bool intersects_ray( const Vector3 & origin, const Vector3 & dir ) const;
    bool intersects_ray_face( const Vector3 & origin, const Vector3 & dir, real_t & face_dist, FaceProperties & fp ) const;
    bool intersects_segment( const Vector3 & start, const Vector3 & end ) const;
    bool intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & face_dist, Vector3 & at, FaceProperties & fp ) const;

    // For visualization.
    PoolVector<Vector3> lines();
	void face_lines( Vector<Vector3> & ret ) const;

    bool parent( const OctreeMeshNode & node, OctreeMeshNode * & parent );

    int  insert_node( OctreeMeshNode & node );
    void update_node( const OctreeMeshNode & node );

    void compute_face_properties();

    int faces_qty() const;
    FaceProperties face_properties( int ind ) const;
	FaceProperties face_properties_world( int ind ) const;

    Vector<OctreeMeshNode> nodes_;
    Vector<Face3>          faces_;
    Vector<FaceProperties> face_props_;

    // Maximum subdivision level.
    int    max_depth_;
    int    min_faces_;

    Vector3 origin_;
    Quat    quat_;
};


}




#endif





