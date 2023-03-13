
#include "marching_cubes_dual.h"
#include "volume_source.h"
#include "cube_tables.h"
#include "vector3d.h"
#include "marching_cubes_dual_node.h"
#include "marching_cubes_dual_cell.h"
#include "volume_node_size_strategy.h"

#include <cmath>
#include <algorithm>
#include <iterator>



namespace Ign
{

bool operator<( const DualCellOctreeNodePair & a, const DualCellOctreeNodePair & b )
{
	const bool ret = ( a.node < b.node );
	return ret;
}

bool operator<( const OctreeNodeFaceIndexPair & a, const OctreeNodeFaceIndexPair & b )
{
	const bool ret = ( a.node < b.node );
	return ret;
}



MarchingCubesDual::MarchingCubesDual()
{
	iso_level     = 0.0;
	max_diff  = 0.2;
	eps           = 1.0e-4;
	step          = 1.0;
	max_nodes_qty = -1;
}


MarchingCubesDual::~MarchingCubesDual()
{
	cleanup_nodes();
}

void MarchingCubesDual::set_min_step( Float step )
{
	this->step = step;
}

Float MarchingCubesDual::get_min_step() const
{
	return step;
}

Float MarchingCubesDual::init_min_step( VolumeSource * source )
{
	step = source->min_node_size() * 0.5;
	return step;
}

MarchingCubesDualNode MarchingCubesDual::create_bounding_node( const Vector3d & contains_pt, Float desired_size ) const
{
	const Integer node_size_int = closest_int_size( desired_size );
	const VectorInt node_origin = node_int_origin( contains_pt, node_size_int );
	MarchingCubesDualNode node;
	node.at = node_origin;
	node.size = node_size_int;
	node.compute_hashes();
	return node;
}


bool MarchingCubesDual::subdivide_source( VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	cleanup_nodes();

	_values_map.clear();
	_normals_map.clear();

	const Float bounding_radius = source->get_bounding_radius();
    const Integer root_size_int_2 = find_subdivision_levels( bounding_radius, source );
	const Integer root_size_int = root_size_int_2 * 2;

	MarchingCubesDualNode  volume_node;
	volume_node.size = root_size_int;
	volume_node.at   = VectorInt( -root_size_int_2, -root_size_int_2, -root_size_int_2 );

	const bool ret = subdivide_source( volume_node, source, strategy );

    return ret;
}

bool MarchingCubesDual::subdivide_source( const MarchingCubesDualNode & volume_node, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	cleanup_nodes();

	_values_map.clear();
	_normals_map.clear();

	//const Float bounding_radius   = source->get_bounding_radius();
	//const Integer root_size_int_2 = find_subdivision_levels( bounding_radius, source );
	//const Integer root_size_int   = root_size_int_2 * 2;

	MarchingCubesDualNode * root_node = create_node();
	root_node->size = volume_node.size;
	root_node->at   = volume_node.at;
	compute_node_values( *root_node, source, strategy );

	const bool ok = root_node->subdivide( this, source, strategy, 0 );
	if ( !ok )
		return false;
	assign_node_indices();
	root_node->compute_hashes();

	// Cleanup 
	_dual_cell_octree_node_pairs.clear();

	// Creating dual grid cells.
	node_proc( root_node, source, strategy );

	// Check how many dual cells were created.
	// If none, return false.
	{
		const bool empty = _octree_dual_cells.empty();
		if ( empty )
			return false;
	}

	// Cleanup.
	_all_faces.clear();
	_materials_set.clear();

	// Create faces and assign material.
	create_faces_in_dual_grid( source );

	// Check how many faces were created.
	// If none, return false.
	{
		const bool empty = _all_faces.empty();
		if ( empty )
			return false;
	}

	// Go over all nodes and assign faces.
	assign_faces_to_octree_nodes();

	// Normalize normals.
	for ( NormalsMapIterator it=_normals_map.begin(); it!=_normals_map.end(); it++ )
	{
		NormalsAndQty & norms = it->second;
		const Vector3d norm = norms.norms / static_cast<Float>(norms.qty);
		norms.norms = norm;
	}

	// Assign vertex positions, normals, and tangents.
	const int faces_qty = _all_faces.size();
	for ( int i=0; i<faces_qty; i++ )
	{
		NodeFace & face = _all_faces[i];
		//const Face3 & f = face.face;
		const Vector3d norm = face.normal();
		const Vector3d tangent = norm.CrossProduct( face.vertices[2] - face.vertices[0] ).Normalized();
		face.tangent = tangent;
		for ( int j=0; j<3; j++ )
		{
			const Vector3d vert         = face.vertices[j];
			const NodeEdgeInt & edge    = face.node_edges[j];
			NormalsMapConstIterator it  = _normals_map.find( edge );
			const NormalsAndQty & n_qty = it->second;
			const Vector3d & n          = n_qty.norms;

			face.normals[j] = n;
		}
	}

	return true;
}

const std::vector<int> & MarchingCubesDual::query_close_nodes( const Vector3d & at_in_source, Float dist, Float max_size )
{
	_octree_node_indices_result.clear();

	const bool empty = _octree_nodes.empty();
	if ( empty )
		return _octree_node_indices_result;

	MarchingCubesDualNode * root = _octree_nodes[0];

	int   sz_int = 1;
	Float sz = step;
	while ( sz < max_size )
	{
		const Float sz_2 = sz * 2.0;
		if ( sz_2 > max_size )
			break;

		sz = sz_2;
		sz_int *= 2;
	}

	const int sz_2 = static_cast<int>( std::ceil( dist / step ) ) * 2;

	//const Vector3d center = inverted_source_se3.q_ * at + inverted_source_se3.r_;
	const Vector3d center = at_in_source;
	const VectorInt at_int = VectorInt( static_cast<int>( (center.x_ - dist) / step ),
		                                static_cast<int>( (center.y_ - dist) / step ),
		                                static_cast<int>( (center.z_ - dist) / step ) );

	MarchingCubesDualNode node;
	node.at   = at_int;
	node.size = sz_2;

	root->query_nodes( node, sz_int, _octree_node_indices_result );
	return _octree_node_indices_result;
}

Vector3d MarchingCubesDual::center_direction( const SE3 & source_se3, const Vector3d & at ) const
{
	const Vector3d full = (source_se3.r_ - at);
	const Float    L    = full.Length();

	const Vector3d a = ( L > 0.00001 ) ? full/L : full;
	return a;
}

MarchingCubesDualNode * MarchingCubesDual::get_tree_node( int ind )
{
	if ( ind < 0 )
		return nullptr;

	if ( ind >= _octree_nodes.size() )
		return nullptr;

	MarchingCubesDualNode * ret = _octree_nodes[ind];
	return ret;
}

bool MarchingCubesDual::point_inside_node( int node_ind, const Vector3d & at_in_source )
{
	Vector3d center;
	MarchingCubesDualNode * node = get_tree_node( node_ind );

	const bool ret = node->contains_point( this, at_in_source );
	return ret;
}

bool MarchingCubesDual::intersect_with_segment( MarchingCubesDualNode * node, const Vector3d & start, const Vector3d & end, Vector3d & at, Vector3d & norm )
{
	if ( node == nullptr )
	{
		const bool empty = _octree_nodes.empty();
		if ( empty )
			return false;

		node = _octree_nodes[0];
	}

	const Vector3d local_start = start;
	const Vector3d local_end   = end;

	const bool ret = node->intersect_with_segment( this, local_start, local_end, at, norm );

	return ret;
}

bool MarchingCubesDual::intersect_with_ray( MarchingCubesDualNode * node, const Vector3d & start, const Vector3d & dir, Vector3d & at, Vector3d & norm )
{
	if ( node == nullptr )
	{
		const bool empty = _octree_nodes.empty();
		if ( empty )
			return false;

		node = _octree_nodes[0];
	}

	const Vector3d local_start = start;
	const Vector3d local_dir   = dir;

	const bool ret = node->intersect_with_ray( this, local_start, local_dir, at, norm );

	return ret;
}


void MarchingCubesDual::set_split_precision( Float rel_diff )
{
	max_diff = rel_diff;
}

Float MarchingCubesDual::get_split_precision() const
{
	return max_diff;
}

bool MarchingCubesDual::should_split( MarchingCubesDualNode * node, int level, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	if (strategy != nullptr)
	{
		if (level >= strategy->get_max_level() )
			return false;
	}
	const VectorInt center_int = node->center();
	const Vector3d center      = at_in_source( center_int, strategy );
	const Float max_sz         = source->max_node_size_at( center );
	const Float min_sz         = source->min_node_size_at( center );

	const Float node_sz = node_size( node );

	// If bigger than maximum allowed, subdivide.
	if ( node_sz > max_sz )
	{
		// Center value.
		const Float center_value = value_at( source, center_int, center );
		const Float abs_center_value = std::abs(center_value);
		// Now check max distance to all the corners. It may be different in warped space.
		bool too_far = true;
		for ( int i=0; i<8; i++ )
		{
			const Vector3d at = node->vertices[i];
			const Vector3d d_at = at - center;
			const Float dist = d_at.Length();
			// If the distance is bigger than the distance to the surface,
			// the node may contain the surface and should be split.
			if ( dist >= abs_center_value )
			{
				too_far = false;
				break;
			}
		}
		// If all the istances are smaller that the distance to the surface,
		// the node cannot contain surface.
		if ( too_far )
			return false;
	}

	// If smaller than minimum allowed, don't subdivide.
	if ( node_sz <= min_sz )
		return false;

	// The size is in between. Use a heuristic.
	const int sz  = node->size;
	const int sz2 = node->size / 2;
	//const VectorInt & c = node->at;
	//const VectorInt[] = {
	//	VectorInt( c + VectorInt( sz2,   0,   0 ) ),
	//	VectorInt( c + VectorInt(   0,   0, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2,   0, sz2 ) ),
	//	VectorInt( c + VectorInt( sz,    0, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2,   0,  sz ) ),

	//	VectorInt( c + VectorInt( sz2, sz2,   0 ) ),
	//	VectorInt( c + VectorInt(   0, sz2, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2, sz2, sz2 ) ),
	//	VectorInt( c + VectorInt( sz,  sz2, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2, sz2,  sz ) ),

	//	VectorInt( c + VectorInt(   0, sz2,   0 ) ),
	//	VectorInt( c + VectorInt(   0, sz2,  sz ) ),
	//	VectorInt( c + VectorInt(  sz, sz2,  sz ) ),
	//	VectorInt( c + VectorInt(  sz, sz2,   0 ) ),

	//	VectorInt( c + VectorInt( sz2,  sz,   0 ) ),
	//	VectorInt( c + VectorInt(   0,  sz, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2,  sz, sz2 ) ),
	//	VectorInt( c + VectorInt( sz,   sz, sz2 ) ),
	//	VectorInt( c + VectorInt( sz2,  sz,  sz ) ),
	//};
	//const int qty = 19;

	// But first check if the node has the surface.
	// If not, there is no point subdividing.
	//const bool has_surface = node->has_surface( iso_level );
	//if ( !has_surface )
	//	return false;

	// For now just the central point.
	Float interpolated_value = 0.0;
	for ( int i=0; i<8; i++ )
		interpolated_value += node->values[i];
	interpolated_value /= 8.0;

	const Vector3d center_float = at_in_source( center_int, strategy );
	const Float    actual_value = value_at( source, center_int, center_float );

	//const Float rel_diff    = std::abs( actual_value - interpolated_value ) / node_sz;
	const Float abs_diff    = std::abs( actual_value - interpolated_value );
	// Compare this difference per unit node size with the threshold.
	const bool do_split = ( abs_diff > max_diff );

	//if ( !do_split )
	//	int iii = 0;

	return do_split;
}

MarchingCubesDualNode * MarchingCubesDual::create_node()
{
	MarchingCubesDualNode * node = memnew( MarchingCubesDualNode );
	_octree_nodes.push_back( node );
	return node;
}


MarchingCubesDualCell * MarchingCubesDual::create_dual_cell()
{
	MarchingCubesDualCell * cell = memnew( MarchingCubesDualCell );
	_octree_dual_cells.push_back( cell );
	return cell;
}

SE3 MarchingCubesDual::se3_in_point( const Vector3d & at ) const
{
	const Vector3d at_s       = at;
	const Vector3d up         = ( at_s / at_s.Length() );
	const Vector3d up_default = Vector3d( 0.0, 1.0, 0.0 );
	const Quaterniond q       = Quaterniond( up_default, up );

	SE3 ret;
	ret.q_ = q;
	ret.r_ = at;

	return ret;
}

SE3 MarchingCubesDual::asset_se3( const SE3 & src_se3, const SE3 & asset_at ) const
{
	const SE3 asset_in_world_se3 = src_se3 * asset_at;

	const SE3 inv_source_se3 = src_se3.inverse();
	const SE3 asset_in_source_se3   = inv_source_se3 * asset_in_world_se3;

	return asset_in_source_se3;
}

const std::set<int> & MarchingCubesDual::materials() const
{
	return _materials_set;
}

const std::vector<Vector3> & MarchingCubesDual::vertices( const SE3 & src_se3, int material_ind, Float scale )
{
	// central_point is a point in source ref. frame with
	// respect to which all vertices are computed.
	// It is done to allow to maximize precision of the
	// surface in the point where the player is located.
	const unsigned int qty = _all_faces.size();
	_ret_verts.clear();
	_ret_verts.reserve(3*qty);

	for ( unsigned int i=0; i<qty; i++ )
	{
		const NodeFace & f = _all_faces[i];
		if (material_ind >= 0)
		{
			const int face_material_ind = f.material_ind;
			if (face_material_ind != material_ind)
				continue;
		}

		for ( int j=0; j<3; j++ )
		{
			const Vector3d source_v = f.vertices[j];
			// Convert to world.
			Vector3d world_v = src_se3 * source_v;
			// Convert relative to scaled origin.
			if ( scale > 0.0 )
			{
				const Vector3d scaled_world_v = scale * world_v;
				_ret_verts.push_back( Vector3( scaled_world_v.x_, scaled_world_v.y_, scaled_world_v.z_ ) );
			}
			else
			{
				_ret_verts.push_back( Vector3( world_v.x_, world_v.y_, world_v.z_ ) );
			}
		}
	}
	return _ret_verts;
}

const std::vector<Vector3> & MarchingCubesDual::normals( const SE3 & src_se3, int material_ind )
{
	const unsigned int qty = _all_faces.size();
	_ret_norms.clear();
	_ret_norms.reserve(3*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const NodeFace & f = _all_faces[i];
		if (material_ind >= 0)
		{
			const int face_material_ind = f.material_ind;
			if (face_material_ind != material_ind)
				continue;
		}

		for ( int j=0; j<3; j++ )
		{
			const Vector3d & v = f.normals[j];
			const Vector3d transformed_v = src_se3.q_ * v;
			_ret_norms.push_back( Vector3( transformed_v.x_, transformed_v.y_, transformed_v.z_ ) );
		}
	}
	return _ret_norms;
}

const std::vector<real_t>  & MarchingCubesDual::tangents( const SE3 & src_se3, int material_ind )
{
	const unsigned int qty = _all_faces.size();
	_ret_tangs.clear();
	_ret_verts.reserve(4*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const NodeFace & f = _all_faces[i];
		if (material_ind >= 0)
		{
			const int face_material_ind = f.material_ind;
			if (face_material_ind != material_ind)
				continue;
		}

		const Vector3d & t = f.tangent;
		const Vector3d transformed_t = src_se3.q_ * t;
		_ret_tangs.push_back( transformed_t.x_ );
		_ret_tangs.push_back( transformed_t.y_ );
		_ret_tangs.push_back( transformed_t.z_ );
		_ret_tangs.push_back( 1.0 );
	}
	return _ret_tangs;
}


void MarchingCubesDual::uvs( int material_ind, const std::vector<Vector2> * & ret_uvs, const std::vector<Vector2> * & ret_uv2s )
{
	const unsigned int qty = _all_faces.size();
	_ret_uvs.clear();
	_ret_uv2s.clear();
	_ret_uvs.reserve(3*qty);
	_ret_uv2s.reserve(3*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const NodeFace & f = _all_faces[i];
		if (material_ind >= 0)
		{
			const int face_material_ind = f.material_ind;
			if (face_material_ind != material_ind)
				continue;
		}

		for ( int j=0; j<3; j++ )
		{
			const Vector3d & v = f.vertices[j];
			_ret_uvs.push_back( Vector2( v.x_, v.y_ ) );
			_ret_uv2s.push_back( Vector2( v.z_, v.z_ ) );
		}
	}
	ret_uvs  = &_ret_uvs;
	ret_uv2s = &_ret_uv2s;
}

int MarchingCubesDual::precompute_scaled_values( const SE3 & src_se3, int material_ind, Float scale )
{
	const std::vector<Vector3> & verts = vertices( src_se3, material_ind, scale );
	const int qty = static_cast<int>( verts.size() );
	normals( src_se3, material_ind );
	tangents( src_se3, material_ind );

	{
		const unsigned int qty = _all_faces.size();
		_ret_uvs.clear();
		_ret_uv2s.clear();
		_ret_uvs.reserve(3*qty);
		_ret_uv2s.reserve(3*qty);
		for ( unsigned int i=0; i<qty; i++ )
		{
			const NodeFace & f = _all_faces[i];
			if (material_ind >= 0)
			{
				const int ind = f.material_ind;
				if (ind != material_ind)
					continue;
			}
			for ( int j=0; j<3; j++ )
			{
				const Vector3d & v = f.vertices[j];
				_ret_uvs.push_back( Vector2( v.x_, v.y_ ) );
				_ret_uv2s.push_back( Vector2( v.z_, v.z_ ) );
			}
		}
	}

	return qty;
}

const std::vector<Vector3> & MarchingCubesDual::vertices() const
{
	return _ret_verts;
}

const std::vector<Vector3> & MarchingCubesDual::normals() const
{
	return _ret_norms;
}

const std::vector<real_t>  & MarchingCubesDual::tangents() const
{
	return _ret_tangs;
}

const std::vector<Vector2> & MarchingCubesDual::uvs() const
{
	return _ret_uvs;
}

const std::vector<Vector2> & MarchingCubesDual::uv2s() const
{
	return _ret_uv2s;
}

const std::vector<Vector3> & MarchingCubesDual::collision_faces( const SE3 & src_se3, const Float dist )
{
	_face_indices_set.clear();
	_faces_ret.clear();

	const bool empty = _octree_nodes.empty();
	if ( empty )
		return _faces_ret;

	MarchingCubesDualNode * root = _octree_nodes[0];

	int dist_int = static_cast<int>( dist / step );
	Float sz = step;
	while ( sz < dist )
	{
		const Float sz_2 = sz * 2.0;
		if ( sz_2 > dist )
			break;

		sz = sz_2;
		dist_int *= 2;
	}

	const SE3 view_point_se3   = src_se3.inverse();
	const Vector3d center      = view_point_se3.r_;
	const int size_int         = static_cast<int>( dist / step );
	const VectorInt center_int = VectorInt( static_cast<int>( center.x_ / step ) - size_int,
		                                    static_cast<int>( center.y_ / step ) - size_int,
		                                    static_cast<int>( center.z_ / step ) - size_int );

	MarchingCubesDualNode node;
	node.at   = center_int;
	node.size = size_int*2;

	{
		int faces_qty_total = 0;
		const int qty = _octree_nodes.size();
		for ( int i=0; i<qty; i++ )
		{
			MarchingCubesDualNode * n = _octree_nodes[i];
			const int faces_qty = n->faces_qty;
			faces_qty_total += faces_qty;
			if ( faces_qty > 0 )
			{
				int j=0;
			}
		}
	}

	root->query_faces( this, node, _face_indices_set );
	for ( MaterialsSet::const_iterator it=_face_indices_set.begin(); it!=_face_indices_set.end(); it++ )
	{
		const int face_ind = *it;
		const NodeFace & nf = _all_faces[face_ind];

		for ( int i=0; i<3; i++ )
		{
			const Vector3d & source_v = nf.vertices[i];
			const Vector3d world_v    = src_se3 * source_v;
			_faces_ret.push_back(  Vector3( world_v.x_, world_v.y_, world_v.z_ ) );
		}
	}

	return _faces_ret;
}








Float MarchingCubesDual::node_size_min( const MarchingCubesDualNode * node ) const
{
	 Float min_sz = (node->vertices[0] - node->vertices[1]).Length();

	 const Float sz_12 = (node->vertices[1] - node->vertices[2]).Length();
	 if ( min_sz > sz_12 )
		 min_sz = sz_12;

	 const Float sz_23 = (node->vertices[2] - node->vertices[3]).Length();
	 if ( min_sz > sz_23 )
		 min_sz = sz_23;

	 const Float sz_30 = (node->vertices[3] - node->vertices[0]).Length();
	 if ( min_sz > sz_30 )
		 min_sz = sz_30;

	 const Float sz_04 = (node->vertices[0] - node->vertices[4]).Length();
	 if ( min_sz > sz_04 )
		 min_sz = sz_04;

	 const Float sz_15 = (node->vertices[1] - node->vertices[5]).Length();
	 if ( min_sz > sz_15 )
		 min_sz = sz_15;

	 const Float sz_26 = (node->vertices[2] - node->vertices[6]).Length();
	 if ( min_sz > sz_26 )
		 min_sz = sz_26;

	 const Float sz_37 = (node->vertices[3] - node->vertices[7]).Length();
	 if ( min_sz > sz_37 )
		 min_sz = sz_37;

	 const Float sz_45 = (node->vertices[4] - node->vertices[5]).Length();
	 if ( min_sz > sz_45 )
		 min_sz = sz_45;

	 const Float sz_56 = (node->vertices[5] - node->vertices[6]).Length();
	 if ( min_sz > sz_56 )
		 min_sz = sz_56;

	 const Float sz_67 = (node->vertices[6] - node->vertices[7]).Length();
	 if ( min_sz > sz_67 )
		 min_sz = sz_67;

	 const Float sz_74 = (node->vertices[7] - node->vertices[4]).Length();
	 if ( min_sz > sz_74 )
		 min_sz = sz_74;

	 return min_sz;
}

Float MarchingCubesDual::node_size_max( const MarchingCubesDualNode * node ) const
{
	Float max_sz = (node->vertices[0] - node->vertices[1]).Length();

	const Float sz_12 = (node->vertices[1] - node->vertices[2]).Length();
	if ( max_sz < sz_12 )
		max_sz = sz_12;

	const Float sz_23 = (node->vertices[2] - node->vertices[3]).Length();
	if ( max_sz < sz_23 )
		max_sz = sz_23;

	const Float sz_30 = (node->vertices[3] - node->vertices[0]).Length();
	if ( max_sz < sz_30 )
		max_sz = sz_30;

	const Float sz_04 = (node->vertices[0] - node->vertices[4]).Length();
	if ( max_sz < sz_04 )
		max_sz = sz_04;

	const Float sz_15 = (node->vertices[1] - node->vertices[5]).Length();
	if ( max_sz < sz_15 )
		max_sz = sz_15;

	const Float sz_26 = (node->vertices[2] - node->vertices[6]).Length();
	if ( max_sz < sz_26 )
		max_sz = sz_26;

	const Float sz_37 = (node->vertices[3] - node->vertices[7]).Length();
	if ( max_sz < sz_37 )
		max_sz = sz_37;

	const Float sz_45 = (node->vertices[4] - node->vertices[5]).Length();
	if ( max_sz < sz_45 )
		max_sz = sz_45;

	const Float sz_56 = (node->vertices[5] - node->vertices[6]).Length();
	if ( max_sz < sz_56 )
		max_sz = sz_56;

	const Float sz_67 = (node->vertices[6] - node->vertices[7]).Length();
	if ( max_sz < sz_67 )
		max_sz = sz_67;

	const Float sz_74 = (node->vertices[7] - node->vertices[4]).Length();
	if ( max_sz < sz_74 )
		max_sz = sz_74;

	return max_sz;
}




Vector3d MarchingCubesDual::at_in_source( const VectorInt & at_i, VolumeNodeSizeStrategy * strategy ) const
{
    const Vector3d at( static_cast<Float>(at_i.x)*step, 
                       static_cast<Float>(at_i.y)*step, 
                       static_cast<Float>(at_i.z)*step );
	if ( strategy != nullptr )
	{
		const Vector3d warped_at = strategy->warp( at );
		return warped_at;
	}
    return at;
}

VectorInt MarchingCubesDual::vector_int( const Vector3d & at ) const
{
	const VectorInt at_int( static_cast<Integer>( std::floor( at.x_ / step ) ),
		                    static_cast<Integer>( std::floor( at.y_ / step ) ),
		                    static_cast<Integer>( std::floor( at.z_ / step ) ) );
	return at_int;
}

Integer MarchingCubesDual::closest_int_size( Float sz ) const
{
	const Float rel_sz = sz / step;
	const Integer int_rel_sz = static_cast<Integer>( std::round( rel_sz ) );
	Integer ret = 2; // "step" corresponds to the size of 2.
	while (ret < int_rel_sz)
		ret *= 2;

	return ret;
}

VectorInt MarchingCubesDual::node_int_origin( const Vector3d & at, Integer node_size_int ) const
{
	const VectorInt at_int = vector_int( at );
	VectorInt o;
	o.x = ( at_int.x >= 0 ) ?
				( at_int.x / node_size_int ) :
		        ( -(node_size_int - at_int.x) / node_size_int );
	o.y = ( at_int.y >= 0 ) ?
		( at_int.y / node_size_int ) :
		( -(node_size_int - at_int.y) / node_size_int );
	o.z = ( at_int.z >= 0 ) ?
		( at_int.z / node_size_int ) :
		( -(node_size_int - at_int.z) / node_size_int );

	o *= node_size_int;

	return o;
}


Float MarchingCubesDual::node_size( const MarchingCubesDualNode * node ) const
{
	const Float ret = step * static_cast<Float>( node->size );
	return ret;
}

int  MarchingCubesDual::get_nodes_qty() const
{
	const int qty = _octree_nodes.size();
	return qty;
}

void MarchingCubesDual::get_node( int node_ind, Vector3d * corners ) const
{
	const MarchingCubesDualNode * node = _octree_nodes[node_ind];
	for ( int i=0; i<8; i++ )
	{
		const Vector3d v = node->vertices[i];
		corners[i] = v;
	}
}

int  MarchingCubesDual::get_node_parent( int node_ind ) const
{
	const MarchingCubesDualNode * node = _octree_nodes[node_ind];
	const MarchingCubesDualNode * parent_node = node->parent_node;
	std::vector<MarchingCubesDualNode *>::const_iterator it = std::find( _octree_nodes.begin(), _octree_nodes.end(), parent_node );
	if ( it == _octree_nodes.end() )
		return -1;
	const int ind = std::distance( _octree_nodes.begin(), it );
	return ind;
}

int  MarchingCubesDual::get_dual_cells_qty() const
{
	const int qty = _octree_dual_cells.size();
	return qty;
}

void MarchingCubesDual::get_dual_cell( int cell_ind, Vector3d * corners ) const
{
	const MarchingCubesDualCell * cell = _octree_dual_cells[cell_ind];
	for ( int i=0; i<8; i++ )
	{
		const Vector3d v = cell->vertices[i];
		corners[i] = v;
	}
}

void MarchingCubesDual::cleanup_nodes()
{
	{
		const int qty = _octree_nodes.size();
		for ( int i=0; i<qty; i++ )
		{
			MarchingCubesDualNode * n = _octree_nodes[i];
			memfree( n );
		}
		_octree_nodes.clear();
	}

	{
		const int qty = _octree_dual_cells.size();
		for ( int i=0; i<qty; i++ )
		{
			MarchingCubesDualCell * c = _octree_dual_cells[i];
			memfree( c );
		}
		_octree_dual_cells.clear();
	}
}

int MarchingCubesDual::find_subdivision_levels( Float bounding_radius, VolumeSource * source )
{
	// Minimum node size should correspond to integer size of 2.
	if ( step <= 0.0 )
		step = source->min_node_size() * 0.5;
	const Float max_sz = bounding_radius;

	// Integer size of the half of minimum size should be 1.
	Integer size_int = 1;
	Float sz = step;
	while (sz <= max_sz)
	{
		sz *= 2.0;
		size_int *= 2;
	}

	return size_int;
}



void MarchingCubesDual::compute_node_values( MarchingCubesDualNode & node, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
    VectorInt verts[8];
    const Integer x  = node.at.x;
    const Integer y  = node.at.y;
    const Integer z  = node.at.z;
    const Integer sz = node.size;
    verts[0] = VectorInt( x,      y,      z );
    verts[1] = VectorInt( x + sz, y,      z );
    verts[2] = VectorInt( x + sz, y,      z + sz );
    verts[3] = VectorInt( x,      y,      z + sz );
    verts[4] = VectorInt( x,      y + sz, z );
    verts[5] = VectorInt( x + sz, y + sz, z );
    verts[6] = VectorInt( x + sz, y + sz, z + sz );
    verts[7] = VectorInt( x,      y + sz, z + sz );

    for ( int i=0; i<8; i++ )
    {
		const VectorInt & vert_int = verts[i];
		node.vertices_int[i]       = verts[i];
        // Value at corner position.
		const Vector3d vert_d      = at_in_source( verts[i], strategy );
		const Float    value       = value_at( source, vert_int, vert_d );
		node.values[i]             = value;
		// Store corner position.
		node.vertices[i]  = vert_d;
    }
}

void MarchingCubesDual::assign_node_indices()
{
	const int qty = _octree_nodes.size();
	for ( int i=0; i<qty; i++ )
	{
		MarchingCubesDualNode * n = _octree_nodes[i];
		n->self_index = i;
	}
}



void MarchingCubesDual::node_proc( const MarchingCubesDualNode * n, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	if (n->has_children())
	{

		const MarchingCubesDualNode *c0 = n->child_nodes[0];
		const MarchingCubesDualNode *c1 = n->child_nodes[1];
		const MarchingCubesDualNode *c2 = n->child_nodes[2];
		const MarchingCubesDualNode *c3 = n->child_nodes[3];
		const MarchingCubesDualNode *c4 = n->child_nodes[4];
		const MarchingCubesDualNode *c5 = n->child_nodes[5];
		const MarchingCubesDualNode *c6 = n->child_nodes[6];
		const MarchingCubesDualNode *c7 = n->child_nodes[7];

		node_proc( c0, source, strategy );
		node_proc( c1, source, strategy );
		node_proc( c2, source, strategy );
		node_proc( c3, source, strategy );
		node_proc( c4, source, strategy );
		node_proc( c5, source, strategy );
		node_proc( c6, source, strategy );
		node_proc( c7, source, strategy );

		face_proc_xy(c0, c3, source, strategy );
		face_proc_xy(c1, c2, source, strategy );
		face_proc_xy(c4, c7, source, strategy );
		face_proc_xy(c5, c6, source, strategy );

		face_proc_zy(c0, c1, source, strategy );
		face_proc_zy(c3, c2, source, strategy );
		face_proc_zy(c4, c5, source, strategy );
		face_proc_zy(c7, c6, source, strategy );

		face_proc_xz(c4, c0, source, strategy );
		face_proc_xz(c5, c1, source, strategy );
		face_proc_xz(c7, c3, source, strategy );
		face_proc_xz(c6, c2, source, strategy );

		edge_proc_x(c0, c3, c7, c4, source, strategy );
		edge_proc_x(c1, c2, c6, c5, source, strategy );

		edge_proc_y(c0, c1, c2, c3, source, strategy );
		edge_proc_y(c4, c5, c6, c7, source, strategy );

		edge_proc_z(c7, c6, c2, c3, source, strategy );
		edge_proc_z(c4, c5, c1, c0, source, strategy );

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}


void MarchingCubesDual::face_proc_xy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();

	if ( n0Subdivided || n1Subdivided )
	{
		const MarchingCubesDualNode *c0 = n0Subdivided ? n0->child_nodes[3] : n0;
		const MarchingCubesDualNode *c1 = n0Subdivided ? n0->child_nodes[2] : n0;
		const MarchingCubesDualNode *c2 = n1Subdivided ? n1->child_nodes[1] : n1;
		const MarchingCubesDualNode *c3 = n1Subdivided ? n1->child_nodes[0] : n1;

		const MarchingCubesDualNode *c4 = n0Subdivided ? n0->child_nodes[7] : n0;
		const MarchingCubesDualNode *c5 = n0Subdivided ? n0->child_nodes[6] : n0;
		const MarchingCubesDualNode *c6 = n1Subdivided ? n1->child_nodes[5] : n1;
		const MarchingCubesDualNode *c7 = n1Subdivided ? n1->child_nodes[4] : n1;

		face_proc_xy(c0, c3, source, strategy);
		face_proc_xy(c1, c2, source, strategy);
		face_proc_xy(c4, c7, source, strategy);
		face_proc_xy(c5, c6, source, strategy);

		edge_proc_x(c0, c3, c7, c4, source, strategy);
		edge_proc_x(c1, c2, c6, c5, source, strategy);
		edge_proc_y(c0, c1, c2, c3, source, strategy);
		edge_proc_y(c4, c5, c6, c7, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}

void MarchingCubesDual::face_proc_zy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();

	if (n0Subdivided || n1Subdivided)
	{
		const MarchingCubesDualNode *c0 = n0Subdivided ? n0->child_nodes[1] : n0;
		const MarchingCubesDualNode *c1 = n1Subdivided ? n1->child_nodes[0] : n1;
		const MarchingCubesDualNode *c2 = n1Subdivided ? n1->child_nodes[3] : n1;
		const MarchingCubesDualNode *c3 = n0Subdivided ? n0->child_nodes[2] : n0;

		const MarchingCubesDualNode *c4 = n0Subdivided ? n0->child_nodes[5] : n0;
		const MarchingCubesDualNode *c5 = n1Subdivided ? n1->child_nodes[4] : n1;
		const MarchingCubesDualNode *c6 = n1Subdivided ? n1->child_nodes[7] : n1;
		const MarchingCubesDualNode *c7 = n0Subdivided ? n0->child_nodes[6] : n0;

		face_proc_zy(c0, c1, source, strategy);
		face_proc_zy(c3, c2, source, strategy);
		face_proc_zy(c4, c5, source, strategy);
		face_proc_zy(c7, c6, source, strategy);

		edge_proc_y(c0, c1, c2, c3, source, strategy);
		edge_proc_y(c4, c5, c6, c7, source, strategy);
		edge_proc_z(c7, c6, c2, c3, source, strategy);
		edge_proc_z(c4, c5, c1, c0, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}

void MarchingCubesDual::face_proc_xz( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();

	if (n0Subdivided || n1Subdivided)
	{
		const MarchingCubesDualNode *c0 = n1Subdivided ? n1->child_nodes[4] : n1;
		const MarchingCubesDualNode *c1 = n1Subdivided ? n1->child_nodes[5] : n1;
		const MarchingCubesDualNode *c2 = n1Subdivided ? n1->child_nodes[6] : n1;
		const MarchingCubesDualNode *c3 = n1Subdivided ? n1->child_nodes[7] : n1;

		const MarchingCubesDualNode *c4 = n0Subdivided ? n0->child_nodes[0] : n0;
		const MarchingCubesDualNode *c5 = n0Subdivided ? n0->child_nodes[1] : n0;
		const MarchingCubesDualNode *c6 = n0Subdivided ? n0->child_nodes[2] : n0;
		const MarchingCubesDualNode *c7 = n0Subdivided ? n0->child_nodes[3] : n0;

		face_proc_xz(c4, c0, source, strategy);
		face_proc_xz(c5, c1, source, strategy);
		face_proc_xz(c7, c3, source, strategy);
		face_proc_xz(c6, c2, source, strategy);

		edge_proc_x(c0, c3, c7, c4, source, strategy);
		edge_proc_x(c1, c2, c6, c5, source, strategy);
		edge_proc_z(c7, c6, c2, c3, source, strategy);
		edge_proc_z(c4, c5, c1, c0, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}


void MarchingCubesDual::edge_proc_x( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();
	const bool n2Subdivided = n2->has_children();
	const bool n3Subdivided = n3->has_children();

	if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
	{
		const MarchingCubesDualNode *c0 = n0Subdivided ? n0->child_nodes[7] : n0;
		const MarchingCubesDualNode *c1 = n0Subdivided ? n0->child_nodes[6] : n0;
		const MarchingCubesDualNode *c2 = n1Subdivided ? n1->child_nodes[5] : n1;
		const MarchingCubesDualNode *c3 = n1Subdivided ? n1->child_nodes[4] : n1;
		const MarchingCubesDualNode *c4 = n3Subdivided ? n3->child_nodes[3] : n3;
		const MarchingCubesDualNode *c5 = n3Subdivided ? n3->child_nodes[2] : n3;
		const MarchingCubesDualNode *c6 = n2Subdivided ? n2->child_nodes[1] : n2;
		const MarchingCubesDualNode *c7 = n2Subdivided ? n2->child_nodes[0] : n2;

		edge_proc_x(c0, c3, c7, c4, source, strategy);
		edge_proc_x(c1, c2, c6, c5, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}

void MarchingCubesDual::edge_proc_y( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();
	const bool n2Subdivided = n2->has_children();
	const bool n3Subdivided = n3->has_children();

	if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
	{
		const MarchingCubesDualNode *c0 = n0Subdivided ? n0->child_nodes[2] : n0;
		const MarchingCubesDualNode *c1 = n1Subdivided ? n1->child_nodes[3] : n1;
		const MarchingCubesDualNode *c2 = n2Subdivided ? n2->child_nodes[0] : n2;
		const MarchingCubesDualNode *c3 = n3Subdivided ? n3->child_nodes[1] : n3;
		const MarchingCubesDualNode *c4 = n0Subdivided ? n0->child_nodes[6] : n0;
		const MarchingCubesDualNode *c5 = n1Subdivided ? n1->child_nodes[7] : n1;
		const MarchingCubesDualNode *c6 = n2Subdivided ? n2->child_nodes[4] : n2;
		const MarchingCubesDualNode *c7 = n3Subdivided ? n3->child_nodes[5] : n3;

		edge_proc_y(c0, c1, c2, c3, source, strategy);
		edge_proc_y(c4, c5, c6, c7, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}

void MarchingCubesDual::edge_proc_z( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();
	const bool n2Subdivided = n2->has_children();
	const bool n3Subdivided = n3->has_children();

	if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided)
	{
		const MarchingCubesDualNode *c0 = n3Subdivided ? n3->child_nodes[5] : n3;
		const MarchingCubesDualNode *c1 = n2Subdivided ? n2->child_nodes[4] : n2;
		const MarchingCubesDualNode *c2 = n2Subdivided ? n2->child_nodes[7] : n2;
		const MarchingCubesDualNode *c3 = n3Subdivided ? n3->child_nodes[6] : n3;
		const MarchingCubesDualNode *c4 = n0Subdivided ? n0->child_nodes[1] : n0;
		const MarchingCubesDualNode *c5 = n1Subdivided ? n1->child_nodes[0] : n1;
		const MarchingCubesDualNode *c6 = n1Subdivided ? n1->child_nodes[3] : n1;
		const MarchingCubesDualNode *c7 = n0Subdivided ? n0->child_nodes[2] : n0;

		edge_proc_z(c7, c6, c2, c3, source, strategy);
		edge_proc_z(c4, c5, c1, c0, source, strategy);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy );
	}
}


void MarchingCubesDual::vert_proc( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
	                               const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7, VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	const bool n0Subdivided = n0->has_children();
	const bool n1Subdivided = n1->has_children();
	const bool n2Subdivided = n2->has_children();
	const bool n3Subdivided = n3->has_children();
	const bool n4Subdivided = n4->has_children();
	const bool n5Subdivided = n5->has_children();
	const bool n6Subdivided = n6->has_children();
	const bool n7Subdivided = n7->has_children();

	if (n0Subdivided || n1Subdivided || n2Subdivided || n3Subdivided ||
		n4Subdivided || n5Subdivided || n6Subdivided || n7Subdivided)
	{
		const MarchingCubesDualNode *c0 = n0Subdivided ? n0->child_nodes[6] : n0;
		const MarchingCubesDualNode *c1 = n1Subdivided ? n1->child_nodes[7] : n1;
		const MarchingCubesDualNode *c2 = n2Subdivided ? n2->child_nodes[4] : n2;
		const MarchingCubesDualNode *c3 = n3Subdivided ? n3->child_nodes[5] : n3;
		const MarchingCubesDualNode *c4 = n4Subdivided ? n4->child_nodes[2] : n4;
		const MarchingCubesDualNode *c5 = n5Subdivided ? n5->child_nodes[3] : n5;
		const MarchingCubesDualNode *c6 = n6Subdivided ? n6->child_nodes[0] : n6;
		const MarchingCubesDualNode *c7 = n7Subdivided ? n7->child_nodes[1] : n7;

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7, source, strategy);
	}
	else
	{

		//if (!n0->isIsoSurfaceNear() && !n1->isIsoSurfaceNear() && !n2->isIsoSurfaceNear() && !n3->isIsoSurfaceNear() &&
		//	!n4->isIsoSurfaceNear() && !n5->isIsoSurfaceNear() && !n6->isIsoSurfaceNear() && !n7->isIsoSurfaceNear())
		//{
		//	return;
		//}

		VectorInt corners[8];
		corners[0] = n0->center();
		corners[1] = n1->center();
		corners[2] = n2->center();
		corners[3] = n3->center();

		corners[4] = n4->center();
		corners[5] = n5->center();
		corners[6] = n6->center();
		corners[7] = n7->center();

		MarchingCubesDualCell * c = add_dual_cell( corners[0], corners[1], corners[2], corners[3],
			                                       corners[4], corners[5], corners[6], corners[7], source, strategy );
		//DualCellOctreeNodePair ccc = DualCellOctreeNodePair( c, const_cast<MarchingCubesDualNode *>(n0) );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );

			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
		}

		create_border_cells(n0, n1, n2, n3, n4, n5, n6, n7, source, strategy );
	}
}


void MarchingCubesDual::create_border_cells( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
										     const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7, VolumeSource * source, VolumeNodeSizeStrategy * strategy  )
{
	const MarchingCubesDualNode * root = _octree_nodes[0];

	if (n0->at_back( root ) && n1->at_back( root ) && n4->at_back( root ) && n5->at_back( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n0->center_back(), n1->center_back(), n1->center(), n0->center(),
			                                      n4->center_back(), n5->center_back(), n5->center(), n4->center(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
		}

		// Generate back edge border cells
		if (n4->at_top( root ) && n5->at_top( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n4->center_back(), n5->center_back(), n5->center(), n4->center(),
				                                      n4->center_back_top(), n5->center_back_top(), n5->center_top(), n4->center_top(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
			}
			// Generate back top corner cells
			if (n4->at_left( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n4->center_back_left(), n4->center_back(), n4->center(), n4->center_left(),
					                                      n4->corner_4(), n4->center_back_top(), n4->center_top(), n4->center_left_top(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
				}
			}
			if (n5->at_right( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n5->center_back(), n5->center_back_right(), n5->center_right(), n5->center(),
					                                      n5->center_back_top(), n5->corner_5(), n5->center_right_top(), n5->center_top(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
				}
			}
		}
		if (n0->at_bottom( root ) && n1->at_bottom( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n0->center_back_bottom(), n1->center_back_bottom(), n1->center_bottom(), n0->center_bottom(),
				                                      n0->center_back(), n1->center_back(), n1->center(), n0->center(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
			}

			// Generate back bottom corner cells
			if (n0->at_left( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n0->corner_0(), n0->center_back_bottom(), n0->center_bottom(), n0->center_left_bottom(),
					                                      n0->center_back_left(), n0->center_back(), n0->center(), n0->center_left(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
				}
			}
			if (n1->at_right( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n1->center_back_bottom(), n1->corner_1(), n1->center_right_bottom(), n1->center_bottom(),
					                                      n1->center_back(), n1->center_back_right(), n1->center_right(), n1->center(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
				}
			}
		}
	}
	if (n2->at_front( root ) && n3->at_front( root ) && n6->at_front( root ) && n7->at_front( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n3->center(), n2->center(), n2->center_front(), n3->center_front(),
			                                      n7->center(), n6->center(), n6->center_front(), n7->center_front(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
		}
		// Generate front edge border cells
		if (n6->at_top( root ) && n7->at_top( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n7->center(), n6->center(), n6->center_front(), n7->center_front(),
				                                      n7->center_top(), n6->center_top(), n6->center_front_top(), n7->center_front_top(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
			}
			// Generate back bottom corner cells
			if (n7->at_left( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n7->center_left(), n7->center(), n7->center_front(), n7->center_front_left(),
					                                      n7->center_left_top(), n7->center_top(), n7->center_front_top(), n7->corner_7(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
				}
			}
			if (n6->at_right( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n6->center(), n6->center_right(), n6->center_front_right(), n6->center_front(),
					                                      n6->center_top(), n6->center_right_top(), n6->corner_6(), n6->center_front_top(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
				}
			}
		}
		if (n3->at_bottom( root ) && n2->at_bottom( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n3->center_bottom(), n2->center_bottom(), n2->center_front_bottom(), n3->center_front_bottom(), 
				                                      n3->center(), n2->center(), n2->center_front(), n3->center_front(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
			}
			// Generate back bottom corner cells
			if (n3->at_left( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n3->center_left_bottom(), n3->center_bottom(), n3->center_front_bottom(), n3->corner_3(),
					                                      n3->center_left(), n3->center(), n3->center_front(), n3->center_front_left(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
				}
			}
			if (n2->at_right( root ))
			{
				MarchingCubesDualCell * c = add_dual_cell(n2->center_bottom(), n2->center_right_bottom(), n2->corner_2(), n2->center_front_bottom(),
					                                      n2->center(), n2->center_right(), n2->center_front_right(), n2->center_front(), source, strategy );
				if ( c != nullptr )
				{
					_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
				}
			}
		}
	}
	if (n0->at_left( root ) && n3->at_left( root ) && n4->at_left( root ) && n7->at_left( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n0->center_left(), n0->center(), n3->center(), n3->center_left(),
			                                      n4->center_left(), n4->center(), n7->center(), n7->center_left(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
		}
		// Generate left edge border cells
		if (n4->at_top( root ) && n7->at_top( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n4->center_left(), n4->center(), n7->center(), n7->center_left(),
				                                      n4->center_left_top(), n4->center_top(), n7->center_top(), n7->center_left_top(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
			}
		}
		if (n0->at_bottom( root ) && n3->at_bottom( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n0->center_left_bottom(), n0->center_bottom(), n3->center_bottom(), n3->center_left_bottom(),
				                                      n0->center_left(), n0->center(), n3->center(), n3->center_left(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
			}
		}
		if (n0->at_back( root ) && n4->at_back( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n0->center_back_left(), n0->center_back(), n0->center(), n0->center_left(),
				                                      n4->center_back_left(), n4->center_back(), n4->center(), n4->center_left(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
			}
		}
		if (n3->at_front( root ) && n7->at_front( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n3->center_left(), n3->center(), n3->center_front(), n3->center_front_left(),
				                                      n7->center_left(), n7->center(), n7->center_front(), n7->center_front_left(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
			}
		}
	}
	if (n1->at_right( root ) && n2->at_right( root ) && n5->at_right( root ) && n6->at_right( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n1->center(), n1->center_right(), n2->center_right(), n2->center(),
			                                      n5->center(), n5->center_right(), n6->center_right(), n6->center(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
		}
		// Generate right edge border cells
		if (n5->at_top( root ) && n6->at_top( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n5->center(), n5->center_right(), n6->center_right(), n6->center(),
				                                      n5->center_top(), n5->center_right_top(), n6->center_right_top(), n6->center_top(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
			}
		}
		if (n1->at_bottom( root ) && n2->at_bottom( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n1->center_bottom(), n1->center_right_bottom(), n2->center_right_bottom(), n2->center_bottom(),
				                                      n1->center(), n1->center_right(), n2->center_right(), n2->center(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
			}
		}
		if (n1->at_back( root ) && n5->at_back( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n1->center_back(), n1->center_back_right(), n1->center_right(), n1->center(),
				                                      n5->center_back(), n5->center_back_right(), n5->center_right(), n5->center(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
			}
		}
		if (n2->at_front( root ) && n6->at_front( root ))
		{
			MarchingCubesDualCell * c = add_dual_cell(n2->center(), n2->center_right(), n2->center_front_right(), n2->center_front(),
				                                      n6->center(), n6->center_right(), n6->center_front_right(), n6->center_front(), source, strategy );
			if ( c != nullptr )
			{
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
				_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
			}
		}
	}
	if (n4->at_top( root ) && n5->at_top( root ) && n6->at_top( root ) && n7->at_top( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n4->center(), n5->center(), n6->center(), n7->center(),
			                                      n4->center_top(), n5->center_top(), n6->center_top(), n7->center_top(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n4 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n5 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n6 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n7 ) );
		}
	}
	if (n0->at_bottom( root ) && n1->at_bottom( root ) && n2->at_bottom( root ) && n3->at_bottom( root ))
	{
		MarchingCubesDualCell * c = add_dual_cell(n0->center_bottom(), n1->center_bottom(), n2->center_bottom(), n3->center_bottom(),
			                                      n0->center(), n1->center(), n2->center(), n3->center(), source, strategy );
		if ( c != nullptr )
		{
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n0 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n1 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n2 ) );
			_dual_cell_octree_node_pairs.push_back( DualCellOctreeNodePair( c, n3 ) );
		}
	}
}


MarchingCubesDualCell * MarchingCubesDual::add_dual_cell( const VectorInt & c0, const VectorInt & c1, const VectorInt & c2, const VectorInt & c3, 
	                                                      const VectorInt & c4, const VectorInt & c5, const VectorInt & c6, const VectorInt & c7,
	                                                      VolumeSource * source, VolumeNodeSizeStrategy * strategy )
{
	VectorInt corners[8];
	corners[0] = c0;
	corners[1] = c1;
	corners[2] = c2;
	corners[3] = c3;

	corners[4] = c4;
	corners[5] = c5;
	corners[6] = c6;
	corners[7] = c7;

	Vector3d ats[8];
	Float values[8];
	int pos_qty = 0;
	int neg_qty = 0;
	for ( int i=0; i<8; i++ )
	{
		ats[i]     = at_in_source( corners[i], strategy );
		values[i]  = value_at( source, corners[i], ats[i] );
		if ( values[i] > iso_level )
			pos_qty += 1;
		else if ( values[i] < iso_level )
			neg_qty += 1;
	}
	const bool create = ( (pos_qty > 0) && (neg_qty > 0) );
	if ( !create )
		return nullptr;

	MarchingCubesDualCell * cell = create_dual_cell();
	for ( int i=0; i<8; i++ )
	{
		cell->values[i]          = values[i];
		cell->vertices_int[i]    = corners[i];
		cell->vertices[i]        = ats[i];
	}

	return cell;
}


void MarchingCubesDual::create_faces_in_dual_grid( VolumeSource * source )
{
	const int qty = _octree_dual_cells.size();
	for ( int i=0; i<qty; i++ )
	{
		MarchingCubesDualCell * cell = _octree_dual_cells[i];
		const int material_index = cell_material( *cell, source );
		create_faces( *cell, material_index );
		_materials_set.insert( material_index );
	}
}


void MarchingCubesDual::assign_faces_to_octree_nodes()
{
	// Cleanup face index assignments.
	_octree_node_face_indices.clear();

	// Sorting by octree node.
	std::sort( _dual_cell_octree_node_pairs.begin(), _dual_cell_octree_node_pairs.end() );
	// Sort faces by dual cell.
	std::sort( _all_faces.begin(), _all_faces.end() );

	const int faces_qty = _all_faces.size();
	const int qty = _dual_cell_octree_node_pairs.size();
	for ( int i=0; i<qty; i++ )
	{
		DualCellOctreeNodePair & pair = _dual_cell_octree_node_pairs[i];
		MarchingCubesDualCell * cell = pair.cell;
		MarchingCubesDualNode * node = pair.node;

		NodeFace nf;
		nf.cell = cell;

		std::vector<NodeFace>::const_iterator it = std::lower_bound( _all_faces.begin(), _all_faces.end(), nf );
		if ( it == _all_faces.end() )
			continue;

		int face_ind = std::distance<>( _all_faces.cbegin(), it );

		NodeFace node_face = _all_faces[face_ind];
		while ( node_face.cell == cell)
		{
			const OctreeNodeFaceIndexPair node_face_ind( node, face_ind );
			_octree_node_face_indices.push_back( node_face_ind );

			face_ind += 1;
			if ( face_ind >= faces_qty )
				break;
			node_face = _all_faces[face_ind];
		}
	}

	//return;

	// Sort by node pointer.
	std::sort( _octree_node_face_indices.begin(), _octree_node_face_indices.end() );

	// Walk over all nodes. For leaf nodes search for lower bound and count how many the same entries exist.
	MarchingCubesDualNode * current_node = nullptr;
	int faces_per_node = 0;
	const int face_references_qty = _octree_node_face_indices.size();
	for ( int i=0; i<face_references_qty; i++ )
	{
		OctreeNodeFaceIndexPair & pair = _octree_node_face_indices[i];
		MarchingCubesDualNode * node = pair.node;
		if ( node != current_node )
		{
			if ( current_node != nullptr )
			{
				current_node->faces_qty = faces_per_node;
			}
			current_node                  = node;
			current_node->face_base_index = i;
			faces_per_node                = 1;
		}
		else
			faces_per_node += 1;
	}
	// After iterated over all array entries need to assign the last one's qty.
	if ( current_node != nullptr )
	{
		current_node->faces_qty = faces_per_node;
	}
}












Vector3d MarchingCubesDual::interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const
{
    const Float d = val1 - val0;
    const Float ad = (d < 0.0) ? (-d) : d;
    Vector3d ret;
    if (ad < eps)
    {
		ret = (v0 + v1) * 0.5;
    }
    else
    {
        const Float mu = (iso_level - val0) / d;
        ret = v0 + (v1 - v0) * mu;
    }

    return ret;
}


void MarchingCubesDual::create_faces( const MarchingCubesDualCell & cell, int material_index )
{
	uint32_t cube_index = 0;
	for ( int i=0; i<8; i++ )
	{
		const Float value = cell.values[i];
		if ( value >= iso_level )
			cube_index |= (1 << i);
	}
	const int edge = CubeTables::EDGES[cube_index];
	Vector3d intersection_points[12];
	NodeEdgeInt edges_int[12];
	if ( edge & 1 )
	{
		if ( cell.vertices_int[0] == cell.vertices_int[1] )
			return;
		intersection_points[0]  = interpolate( cell.vertices[0], cell.vertices[1], cell.values[0], cell.values[1] );
		edges_int[0] = NodeEdgeInt( cell.vertices_int[0], cell.vertices_int[1] );
	}
	if ( edge & 2 )
	{
		if ( cell.vertices_int[1] == cell.vertices_int[2] )
			return;
		intersection_points[1]  = interpolate( cell.vertices[1], cell.vertices[2], cell.values[1], cell.values[2] );
		edges_int[1] = NodeEdgeInt( cell.vertices_int[1], cell.vertices_int[2] );
	}
	if ( edge & 4 )
	{
		if ( cell.vertices_int[2] == cell.vertices_int[3] )
			return;
		intersection_points[2]  = interpolate( cell.vertices[2], cell.vertices[3], cell.values[2], cell.values[3] );
		edges_int[2] = NodeEdgeInt( cell.vertices_int[2], cell.vertices_int[3] );
	}
	if ( edge & 8 )
	{
		if ( cell.vertices_int[3] == cell.vertices_int[0] )
			return;
		intersection_points[3]  = interpolate( cell.vertices[3], cell.vertices[0], cell.values[3], cell.values[0] );
		edges_int[3] = NodeEdgeInt( cell.vertices_int[3], cell.vertices_int[0] );
	}
	if ( edge & 16 )
	{
		if ( cell.vertices_int[4] == cell.vertices_int[5] )
			return;
		intersection_points[4]  = interpolate( cell.vertices[4], cell.vertices[5], cell.values[4], cell.values[5] );
		edges_int[4] = NodeEdgeInt( cell.vertices_int[4], cell.vertices_int[5] );
	}
	if ( edge & 32 )
	{
		if ( cell.vertices_int[5] == cell.vertices_int[6] )
			return;
		intersection_points[5]  = interpolate( cell.vertices[5], cell.vertices[6], cell.values[5], cell.values[6] );
		edges_int[5] = NodeEdgeInt( cell.vertices_int[5], cell.vertices_int[6] );
	}
	if ( edge & 64 )
	{
		if ( cell.vertices_int[6] == cell.vertices_int[7] )
			return;
		intersection_points[6]  = interpolate( cell.vertices[6], cell.vertices[7], cell.values[6], cell.values[7] );
		edges_int[6] = NodeEdgeInt( cell.vertices_int[6], cell.vertices_int[7] );
	}
	if ( edge & 128 )
	{
		if ( cell.vertices_int[7] == cell.vertices_int[4] )
			return;
		intersection_points[7]  = interpolate( cell.vertices[7], cell.vertices[4], cell.values[7], cell.values[4] );
		edges_int[7] = NodeEdgeInt( cell.vertices_int[7], cell.vertices_int[4] );
	}
	if ( edge & 256 )
	{
		if ( cell.vertices_int[0] == cell.vertices_int[4] )
			return;
		intersection_points[8]  = interpolate( cell.vertices[0], cell.vertices[4], cell.values[0], cell.values[4] );
		edges_int[8] = NodeEdgeInt( cell.vertices_int[0], cell.vertices_int[4] );
	}
	if ( edge & 512 )
	{
		if ( cell.vertices_int[1] == cell.vertices_int[5] )
			return;
		intersection_points[9]  = interpolate( cell.vertices[1], cell.vertices[5], cell.values[1], cell.values[5] );
		edges_int[9] = NodeEdgeInt( cell.vertices_int[1], cell.vertices_int[5] );
	}
	if ( edge & 1024 )
	{
		if ( cell.vertices_int[2] == cell.vertices_int[6] )
			return;
		intersection_points[10] = interpolate( cell.vertices[2], cell.vertices[6], cell.values[2], cell.values[6] );
		edges_int[10] = NodeEdgeInt( cell.vertices_int[2], cell.vertices_int[6] );
	}
	if ( edge & 2048 )
	{
		if ( cell.vertices_int[3] == cell.vertices_int[7] )
			return;
		intersection_points[11] = interpolate( cell.vertices[3], cell.vertices[7], cell.values[3], cell.values[7] );
		edges_int[11] = NodeEdgeInt( cell.vertices_int[3], cell.vertices_int[7] );
	}

	const int * indices = CubeTables::TRIANGLES[cube_index];
	for ( int i=0; indices[i] != -1; i+=3 )
	{
		const int ind_a = indices[i];
		const int ind_b = indices[i+1];
		const int ind_c = indices[i+2];

		const Vector3d & a = intersection_points[ind_a];
		const Vector3d & b = intersection_points[ind_b];
		const Vector3d & c = intersection_points[ind_c];

		const NodeEdgeInt & edge_a = edges_int[ind_a];
		const NodeEdgeInt & edge_b = edges_int[ind_b];
		const NodeEdgeInt & edge_c = edges_int[ind_c];

		//const Vector3 fa( a.x_, a.y_, a.z_ );
		//const Vector3 fb( b.x_, b.y_, b.z_ );
		//const Vector3 fc( c.x_, c.y_, c.z_ );
		//const Face3 f( fa, fb, fc );

		NodeFace face( a, b, c, edge_a, edge_b, edge_c );
		face.cell = const_cast<MarchingCubesDualCell *>( &cell );
		face.material_ind = material_index;
		_all_faces.push_back( face );

		const Vector3d norm = face.normal();
		append_normal( edge_a, norm );
		append_normal( edge_b, norm );
		append_normal( edge_c, norm );
	}
}



Float MarchingCubesDual::value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at )
{
	ValuesMapConstIterator it = _values_map.find( vector_int );
	if ( it != _values_map.end() )
	{
		const Float v = it->second;
		return v;
	}

	const Float v = source->value_global( at );
	_values_map[vector_int] = v;

	return v;
}

int MarchingCubesDual::cell_material( const MarchingCubesDualCell & cell, VolumeSource * source )
{
	int max_priority = -1;
	int max_material = -1;
	for ( int i=0; i<8; i++ )
	{
		const VectorInt & at_i = cell.vertices_int[i];
		const Vector3d    at_d = cell.vertices[i];
		int priority;
		const int material = material_at( source, at_i, at_d, &priority );
		// It is critical that it is "<=" and not just "<". It is because
		// next object should overwrite previous object unless priority is smaller.
		if ( (max_priority < 0) || (max_priority <= priority) )
		{
			max_material = material;
			max_priority = priority;
		}
	}

	return max_material;
}


int MarchingCubesDual::material_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at, int * priority )
{
	MaterialsMapConstIterator it = _materials_map.find( vector_int );
	if ( it != _materials_map.end() )
	{
		const MaterialWithPriorityDual mp = it->second;
		if ( priority != nullptr )
			*priority = mp.priority;
		return mp.material;
	}

	int priority_index;
	const int material_index = source->material_global( at, &priority_index );
	MaterialWithPriorityDual mp;
	mp.material = material_index;
	mp.priority = priority_index;
	_materials_map[vector_int] = mp;

	if ( priority != nullptr )
		*priority = priority_index;

	return material_index;
}




void MarchingCubesDual::append_normal( const NodeEdgeInt & edge, const Vector3d & n )
{
	NormalsMapIterator it = _normals_map.find( edge );
	if ( it != _normals_map.end() )
	{
		NormalsAndQty & norms = it->second;
		norms.qty   += 1;
		norms.norms += n;
		return;
	}

	const NormalsAndQty norms( n );
	_normals_map[edge] = norms;
}

const NodeFace & MarchingCubesDual::get_face_by_index( int ind )
{
	const OctreeNodeFaceIndexPair & p = _octree_node_face_indices[ind];
	const int ind_in_all_faces = p.face_index;
	const NodeFace & ret = _all_faces[ind_in_all_faces];
	return ret;
}




}





