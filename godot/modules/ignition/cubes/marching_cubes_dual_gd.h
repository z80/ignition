
#ifndef __MARCHING_CUBES_DUAL_GD_H_
#define __MARCHING_CUBES_DUAL_GD_H_

#include "marching_cubes_dual.h"
#include "core/reference.h"
#include "se3_ref.h"
#include "volume_source_gd.h"
#include "distance_scaler_ref.h"
#include "marching_cubes_dual_node_gd.h"
#include "marching_cubes_rebuild_strategy_gd.h"

class Node;

namespace Ign
{

class MarchingCubesDualGd: public Reference
{
	GDCLASS(MarchingCubesDualGd, Reference);
protected:
	static void _bind_methods();

public:
	MarchingCubesDualGd();
	~MarchingCubesDualGd();

	//void set_source_transform( const Ref<Se3Ref> & se3 );
	//Ref<Se3Ref> get_source_transform() const;

	void set_split_precision( real_t rel_diff );
	real_t get_split_precision() const;

	bool subdivide_source( real_t bounding_radius, const Ref<VolumeSourceGd> & volume, const Ref<MarchingCubesRebuildStrategyGd> & strategy = Ref<MarchingCubesRebuildStrategyGd>() );

	Array query_close_nodes( const Vector3 & at, real_t dist, real_t max_size, const Ref<Se3Ref> & inv_src_se3=Ref<Se3Ref>() );
	Vector3 center_direction( const Vector3 & at, bool in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<Se3Ref> & inv_src_se3=Ref<Se3Ref>() ) const;
	Ref<MarchingCubesDualNodeGd> get_tree_node( int ind );

	Ref<Se3Ref> se3_in_point( const Vector3 & at, bool in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<Se3Ref> & inv_source_se3=Ref<Se3Ref>() ) const;
	Ref<Se3Ref> asset_se3( const Ref<Se3Ref> & asset_at, bool result_in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() ) const;
	Transform   asset_transform( const Ref<Se3Ref> & asset_at, bool result_in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() ) const;

	Array materials_used();
	// This one computes and applies synchronously.
	void apply_to_mesh( int material_index, Node * mesh_instance, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() );
	// And this one computes and applies in tow different methods.
	void precompute_scaled_values( int material_index, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() );
	void apply_to_mesh_only( Node * mesh_instance );

	Transform mesh_transform( const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() );
	const PoolVector3Array & collision_faces( const Vector3 & at, real_t dist, bool in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>() );

	void set_max_nodes_qty( int qty );
	int get_max_nodes_qty() const;

	// Debug methods.
	int  get_nodes_qty() const;
	Array get_node( int node_ind, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>() ) const;
	int  get_node_parent( int node_ind ) const;

	int  get_dual_cells_qty() const;
	Array get_dual_cell( int cell_ind, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>() ) const;


public:
	MarchingCubesDual cubes;

	PoolVector3Array vertices,
		             normals;
	PoolRealArray    tangents;
	PoolVector2Array uvs,
		             uv2s;

	PoolVector3Array ret_pool_array;
	Array            ret_array;
};




}





#endif







