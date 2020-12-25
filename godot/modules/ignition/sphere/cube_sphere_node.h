
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"

#include "cube_sphere.h"
#include "subdivide_source.h"

#include "height_source_ref.h"


namespace Ign
{

class HeightSourceRef;

class CubeSphereNode: public MeshInstance
{
	GDCLASS( CubeSphereNode, MeshInstance );

protected:
	static void _bind_methods();

public:
	CubeSphereNode();
	~CubeSphereNode();

	void set_height_source( Ref<HeightSourceRef> hs );

	void set_r( real_t r );
	real_t get_r() const;

	void set_h( real_t h );
	real_t get_h() const;

	// Subdivision levels.
	void clear_levels();
	void add_level( real_t sz, real_t dist );

	// Need in sphere rebuild is checked every period.
	void set_subdivision_check_period( real_t sec );
	real_t get_subdivision_check_period() const;

	// Points around which subdivision occurs.
	void clear_points_of_interest();
	void add_point_of_interest( const Vector3 & at, bool close );

	// For debugging check for rebuild and rebuild manually.
	bool need_rebuild();
	void rebuild();
	// All geometry triangles.
	PoolVector3Array triangles();
	// Only collision triangles.
	PoolVector3Array collision_triangles( real_t dist );

public:

	// These all are for geometry generation.
	CubeSphere           sphere;
	SubdivideSource      subdivide_source;
	Ref<HeightSourceRef> height_source;

	real_t check_period;
	Vector<SubdivideSource::SubdividePoint> points_of_interest;

	// All triangles.
	Vector<CubeVertex> all_tris;
	PoolVector3Array vertices;
	PoolVector3Array normals;
	PoolRealArray    tangents;
	PoolColorArray   colors;
	PoolVector2Array uvs;
	PoolVector2Array uvs2;
};


}





#endif



