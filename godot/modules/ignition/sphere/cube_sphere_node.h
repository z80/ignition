
#ifndef __CUBE_SHPERE_NODE_H_
#define __CUBE_SPHERE_NODE_H_

#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/mesh.h"
#include "core/object/ref_counted.h"

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
    //OBJ_CATEGORY("Ignition");

protected:
    static void _bind_methods();

public:
    CubeSphereNode();
    ~CubeSphereNode();

    void set_height_source( const Ref<HeightSourceRef> & hs );
    Ref<HeightSourceRef> get_height_source() const;

    void set_radius( real_t r );
    real_t get_radius() const;

    void set_height( real_t h );
    real_t get_height() const;

    // Subdivision levels.
    void clear_levels();
    void add_level( real_t sz, real_t dist );

    // Only collision triangles.
    // Triangles are in "origin"'s ref frame.
    // Triangles are all triangles closer than "dist" from all "ref_frames".
    const Array & collision_triangles( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref, real_t dist );

    // Content generation faces.
    const Array & content_cells( Node * origin, real_t cell_size, real_t dist );
    Ref<Se3Ref> local_se3( int cell_ind, const Vector2 & unit_at, bool true_surface_normal ) const;
    Ref<Se3Ref> surface_se3( const Vector3 & unit_at, real_t height ) const;

    // Sphere center RefFrame
    void set_target_mesh( const NodePath & path );
    const NodePath & get_target_mesh() const;

    //void set_distance_scaler( const Ref<DistanceScalerRef> & new_scaler );
    //Ref<DistanceScalerRef> get_distance_scaler() const;

    void set_apply_scale( bool en );
    bool get_apply_scale() const;

    // How far "close"/"far" scaling mode shifts from one to another.
    //void set_scale_mode_distance( real_t radie );
    //real_t get_scale_mode_distance() const;

    // For debugging (when origin rf is not in (0,0,0)) need to convert to world rf.
    void set_convert_to_global( bool en );
    bool get_convert_to_global() const;

    //void relocate_mesh( Node * ref_frame, Node * player_ctrl, const Ref<SubdivideSourceRef> & subdivide_source );
    //void rebuild_shape( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source );
    //void rebuild_scale( Node * ref_frame );
    void apply_visual_mesh();

private:

    //void regenerate_mesh( RefFrameNode * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref );
    //void adjust_pose( RefFrameNode * ref_frame, RefFrameNode * player_ctrl, const Ref<SubdivideSourceRef> & subdivide_source );
    void init_levels();

	//void apply_scale( RefFrameNode * ref_frame );
 //   void scale_close( const SE3 center_relative_to_ref_frame, const SE3 & poi_relative_to_center );
 //   void scale_far( const SE3 center_relative_to_ref_frame, const SE3 & poi_relative_to_center );
 //   void scale_neutral( const SE3 & poi_relative_to_center );

	MeshInstance3D * get_mesh_instance();

public:
	void subdivide_2( Node * player_rf, Ref<SubdivideSourceRef> subdivide_source_ref );
	void apply_heightmap_2( const Ref<HeightSourceRef> & hs );
	void apply_scale_2( Node * player_rf, Node * camera_node, Ref<DistanceScalerRef> scaler );

private:
	// Reworking the way mesh is generated.
	void _subdivide_2( RefFrameNode * player_rf, Ref<SubdivideSourceRef> subdivide_source_ref );
	void _apply_heightmap_2( const Ref<HeightSourceRef> & hs );
	void _apply_scale_2( RefFrameNode * player_rf, Node * camera_node, Ref<DistanceScalerRef> scaler );
public:

    // These all are for geometry generation.
    CubeSphere           sphere;
    Ref<HeightSourceRef> height_source;

    // Where to generate sphere.
    NodePath             target_path;

    // These are for determining when subdivision is needed and for placement of
    // points of interest.
    //bool                 generate_close;

    // For querying collisions store all ref frames in this container.
    Vector<CubeVertex> collision_tris;
    Array              collision_ret;

    // Returning content cells.
    Vector<Vector3d> content_pts;
    Vector<int>      content_cell_inds;
    Array            content_cells_ret;

    // All triangles.
    Vector<CubeVertex> all_tris;
    Array   vertices;
    Array   normals;
    Array   tangents;
    Array   colors;
    Array   uvs;
    Array   uvs2;

    // Distance scale.
    // It applies scale depending on the distance.
    //Ref<DistanceScalerRef> scale;
    // Should apply scale at all.
    bool _apply_scale;
    // How far measured in sphere radie scaling mode shifts from "close" to "far"
    // and vise versa.
    //Float scale_mode_distance;

    // convert "center_to_origin" to world rf.
    bool _convert_to_global;
};


}





#endif



