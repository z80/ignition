
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"

#include "cube_sphere.h"
#include "subdivide_source.h"

#include "height_source_ref.h"
#include "se3.h"

namespace Ign
{

class HeightSourceRef;
class RefFrameNode;

class CubeSphereNode: public MeshInstance
{
	GDCLASS( CubeSphereNode, MeshInstance );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();
	void _notification( int p_what );

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


	// Distance scaling.
	void set_close( bool en );
	bool get_close() const;

	// Sphere center RefFrame
	void set_center_ref_frame( const NodePath & path );
	const NodePath & get_center_ref_frame() const;

	// Player physics ref frame.
	void set_origin_ref_frame( const NodePath & path );
	const NodePath & get_origin_ref_frame() const;

	// Adding ref frames of interest (those can be view ref frames or physics ref frames).
	void clear_ref_frames();
	void add_ref_frame( const NodePath & path );
	void remove_ref_frame( const NodePath & path );

	//void set_distance_scaler( Ref<DistanceScalerRef> new_scaler );
	//Ref<DistanceScalerRef> get_distance_scaler() const;

	//void set_apply_scale( bool en );
	//bool get_apply_scale() const;

private:
	// Makes sure that origin ref frame is in the list.
	// Removes nonexisting ones. Created an array of ref frames.
	void validate_ref_frames();

	void process_transform();
	void regenerate_mesh();
	void adjust_pose();

public:

	// These all are for geometry generation.
	CubeSphere           sphere;
	SubdivideSource      subdivide_source;
	Ref<HeightSourceRef> height_source;

	// These are for determining when subdivision is needed and for placement of
	// points of interest.
	bool                   generate_close;
	NodePath               center_path;
	NodePath               origin_path;
	Vector<NodePath>       ref_frame_paths;
	Vector<RefFrameNode *> ref_frames;
	SE3                    poi_relative_to_center;
	SE3                    poi_relative_to_origin;

	real_t check_period;
	Vector<SubdivideSource::SubdividePoint> points_of_interest;

	// All triangles.
	Vector<CubeVertex> all_tris, collision_tris;
	PoolVector3Array   vertices;
	PoolVector3Array   normals;
	PoolRealArray      tangents;
	PoolColorArray     colors;
	PoolVector2Array   uvs;
	PoolVector2Array   uvs2;

	// Distance scale.
	//Ref<DistanceScalerRef> scale;
	//bool apply_scale;
};


}





#endif



