
#ifndef __MARCHING_CUBES_DUAL_GD_H_
#define __MARCHING_CUBES_DUAL_GD_H_

#include "marching_cubes_dual.h"
#include "core/object/ref_counted.h"
#include "se3_ref.h"
#include "volume_source_gd.h"
#include "marching_cubes_dual_node_gd.h"
#include "volume_node_size_strategy_gd.h"
#include "bounding_node_gd.h"

class Node;

namespace Ign
{

class MarchingCubesDualGd: public RefCounted
{
	GDCLASS(MarchingCubesDualGd, RefCounted);
protected:
	static void _bind_methods();

public:
	MarchingCubesDualGd();
	~MarchingCubesDualGd();

	void set_split_precision( real_t rel_diff );
	real_t get_split_precision() const;

	void set_min_step( real_t step );
	real_t get_min_step() const;
	real_t init_min_step( const Ref<VolumeSourceGd> & volume );

	Ref<BoundingNodeGd> create_bounding_node( const Ref<Se3Ref> & contains_pt, real_t desired_size ) const;

	bool subdivide_source_all( const Ref<VolumeSourceGd> & volume, const Ref<VolumeNodeSizeStrategyGd> & strategy = Ref<VolumeNodeSizeStrategyGd>() );
	bool subdivide_source( const Ref<BoundingNodeGd> & bounding_node, const Ref<VolumeSourceGd> & volume, const Ref<VolumeNodeSizeStrategyGd> & strategy = Ref<VolumeNodeSizeStrategyGd>() );

	Array query_close_nodes( const Vector3 & at_in_source, real_t dist, real_t max_size );
	Vector3 center_direction( const Ref<Se3Ref> & src_se3, const Vector3 & at ) const;
	Ref<MarchingCubesDualNodeGd> get_tree_node( int ind );

	Array intersect_with_segment( const Vector3 & start, const Vector3 & end );
	Array intersect_with_ray( const Vector3 & start, const Vector3 & dir );

	Ref<Se3Ref> se3_in_point( const Vector3 & at, const Ref<Se3Ref> & inv_source_se3=Ref<Se3Ref>() ) const;
	Ref<Se3Ref> asset_se3( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_at ) const;
	Transform3D asset_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_at ) const;

	Array materials_used();
	// This one computes and applies synchronously.
	bool apply_to_mesh( const Ref<Se3Ref> & src_se3, int material_index, real_t scale, Node * mesh_instance );
	// And this one computes and applies in tow different methods.
	int precompute_scaled_values( const Ref<Se3Ref> & src_se3, int material_index, real_t scale, const Vector3 & world_pos_bias );
	bool apply_to_mesh_only( Node * mesh_instance );
	bool apply_to_mesh_only_wireframe( Node * mesh_instance );
	bool apply_to_collision_shape( Object * concave_polygon_shape );

	//Ref<Se3Ref> compute_source_se3( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & pt_in_source_se3 );
	//Transform3D compute_source_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & pt_in_source_se3 );
	const Array & collision_faces( const Ref<Se3Ref> & src_se3, real_t dist );

	void set_max_nodes_qty( int qty );
	int get_max_nodes_qty() const;

	// Debug methods.
	int  get_nodes_qty() const;
	Array get_node( int node_ind ) const;
	int  get_node_parent( int node_ind ) const;

	int  get_dual_cells_qty() const;
	Array get_dual_cell( int cell_ind ) const;


public:
	MarchingCubesDual cubes;

	Array vertices,
	      normals;
	Array tangents;
	Array uvs,
	      uv2s;

	Array ret_pool_array;
	Array ret_array;
};




}





#endif







