
#ifndef __OCTREE_MESH_H_
#define __OCTREE_MESH_H_

#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"
#include "octree_mesh_node.h"


/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
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

	// Manipulating filling up the occupancy grid.
	void clear();
	void append( const Transform & t, const Ref<Mesh> mesh );
	void subdivide();

	// Intersects with infinite ray.
	bool intersects_ray( const Vector3 origin, const Vector3 dir ) const;

	// For visualization.
	PoolVector<Vector3> lines();

	bool parent( const OctreeMeshNode & node, OctreeMeshNode * & parent );

	int  insert_node( OctreeMeshNode & node );
	void update_node( const OctreeMeshNode & node );

	void compute_face_properties();

	Vector<OctreeMeshNode> nodes_;
	Vector<Face3>          faces_;
	Vector<FaceProperties> face_props_;

	// Maximum subdivision level.
	real_t node_sz_;
	int    max_depth_;
	int    min_faces_;
};







#endif





