
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"

#include "cube_sphere.h"
#include "subdivide_source.h"

#include "height_source_ref.h"
#include "se3.h"
#include "se3_ref.h"
#include "distance_scaler_ref.h"

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

	// Only collision triangles.
	// Triangles are in "origin"'s ref frame.
	// Triangles are all triangles closer than "dist" from all "ref_frames".
	const PoolVector3Array & collision_triangles( Node * origin, const Array & ref_frames, real_t dist );

	// Content generation faces.
	const Array & content_cells( Node * origin, real_t cell_size, real_t dist );
	Ref<Se3Ref> local_se3( int cell_ind, const Vector2 & unit_at, bool true_surface_normal ) const;
	Ref<Se3Ref> surface_se3( const Vector3 & unit_at ) const;

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

	void set_distance_scaler( Ref<DistanceScalerRef> new_scaler );
	Ref<DistanceScalerRef> get_distance_scaler() const;

	void set_apply_scale( bool en );
	bool get_apply_scale() const;

	// How far "close"/"far" scaling mode shifts from one to another.
	void set_scale_mode_distance( real_t radie );
	real_t get_scale_mode_distance() const;

	// For debugging (when origin rf is not in (0,0,0)) need to convert to world rf.
	void set_convert_to_global( bool en );
	bool get_convert_to_global() const;

private:
	// Makes sure that origin ref frame is in the list.
	// Removes nonexisting ones. Created an array of ref frames.
	void validate_ref_frames();
	bool need_rebuild();

	void process_transform();
	void regenerate_mesh();
	void adjust_pose();
	void init_levels();

	void scale_close();
	void scale_far();
	void scale_neutral();
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
	// Sphere point closest to the observation point relative to sphere center.
	SE3                    poi_relative_to_center;
	// Sphere center relative to observation point.
	SE3                    center_relative_to_origin;

	// For querying collisions store all ref frames in this container.
	Vector<RefFrameNode *> collision_ref_frames;
	Vector<SubdivideSource::SubdividePoint> collision_pts;
	Vector<CubeVertex> collision_tris;
	PoolVector3Array   collision_ret;

	// Returning content cells.
	Vector<SubdivideSource::SubdividePoint> content_pts;
	Vector<int> content_cell_inds;
	Array       content_cells_ret;

	// For checking rebuild status periodically.
	real_t check_period;
	real_t check_time_elapsed;
	Vector<SubdivideSource::SubdividePoint> points_of_interest;

	// All triangles.
	Vector<CubeVertex> all_tris;
	PoolVector3Array   vertices;
	PoolVector3Array   normals;
	PoolRealArray      tangents;
	PoolColorArray     colors;
	PoolVector2Array   uvs;
	PoolVector2Array   uvs2;

	// Distance scale.
	// It applies scale depending on the distance.
	Ref<DistanceScalerRef> scale;
	// Should apply scale at all.
	bool apply_scale;
	// How far measured in sphere radie scaling mode shifts from "close" to "far"
	// and vise versa.
	Float scale_mode_distance;

	// convert "center_to_origin" to world rf.
	bool convert_to_global;
};


}





#endif



