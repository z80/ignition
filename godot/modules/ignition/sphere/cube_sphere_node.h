
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"

#include "cube_sphere.h"
#include "ref_frame_node.h"
#include "subdivide_source_ref.h"
#include "height_source_ref.h"
#include "distance_scaler_ref.h"
#include "se3_ref.h"

namespace Ign
{

class HeightSourceRef;
class RefFrameNode;

class CubeSphereNode: public RefFrameNode
{
    GDCLASS( CubeSphereNode, RefFrameNode );
    OBJ_CATEGORY("Ignition");

protected:
    static void _bind_methods();

public:
    CubeSphereNode();
    ~CubeSphereNode();

    void set_height_source( const Ref<HeightSourceRef> & hs );
    Ref<HeightSourceRef> get_height_source() const;

    void set_r( real_t r );
    real_t get_r() const;

    void set_h( real_t h );
    real_t get_h() const;

    // Subdivision levels.
    void clear_levels();
    void add_level( real_t sz, real_t dist );

    // Only collision triangles.
    // Triangles are in "origin"'s ref frame.
    // Triangles are all triangles closer than "dist" from all "ref_frames".
    const PoolVector3Array & collision_triangles( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref, real_t dist );

    // Content generation faces.
    const Array & content_cells( Node * origin, real_t cell_size, real_t dist );
    Ref<Se3Ref> local_se3( int cell_ind, const Vector2 & unit_at, bool true_surface_normal ) const;
    Ref<Se3Ref> surface_se3( const Vector3 & unit_at, real_t height ) const;

    // Sphere center RefFrame
    void set_target_mesh( const NodePath & path );
    const NodePath & get_target_mesh() const;

    void set_distance_scaler( const Ref<DistanceScalerRef> & new_scaler );
    Ref<DistanceScalerRef> get_distance_scaler() const;

    void set_apply_scale( bool en );
    bool get_apply_scale() const;

    // How far "close"/"far" scaling mode shifts from one to another.
    void set_scale_mode_distance( real_t radie );
    real_t get_scale_mode_distance() const;

    // For debugging (when origin rf is not in (0,0,0)) need to convert to world rf.
    void set_convert_to_global( bool en );
    bool get_convert_to_global() const;

	void relocate_mesh( Node * ref_frame );
	void rebuild_shape( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source );
	void apply_visual_mesh();

private:

    void regenerate_mesh( RefFrameNode * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref );
    void adjust_pose( RefFrameNode * ref_frame );
    void init_levels();

    void scale_close();
    void scale_far();
    void scale_neutral();

	MeshInstance * get_mesh_instance();
public:

    // These all are for geometry generation.
    CubeSphere           sphere;
    Ref<HeightSourceRef> height_source;

    // Where to generate sphere.
    NodePath             target_path;

    // These are for determining when subdivision is needed and for placement of
    // points of interest.
    bool                 generate_close;
    // Sphere point closest to the observation point relative to sphere center.
    SE3                  poi_relative_to_center;
    // Sphere center relative to observation point.
    SE3                  center_relative_to_ref_frame;

    // For querying collisions store all ref frames in this container.
    Vector<CubeVertex> collision_tris;
    PoolVector3Array   collision_ret;

    // Returning content cells.
    Vector<Vector3d> content_pts;
    Vector<int>      content_cell_inds;
    Array            content_cells_ret;

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



