
#include "marching_cubes_dual.h"
#include "volume_source.h"
#include "cube_tables.h"
#include "vector3d.h"
#include "distance_scaler_base.h"
#include "marching_cubes_dual_node.h"

#include <cmath>



namespace Ign
{


MarchingCubesDual::MarchingCubesDual()
{
	iso_level     = 0.0;
	max_rel_diff  = 0.2;
	eps           = 1.0e-4;
	step          = 1.0;
	levels_qty    = 1;
	max_nodes_qty = -1;
}


MarchingCubesDual::~MarchingCubesDual()
{
	cleanup_nodes();
}


void MarchingCubesDual::set_source_transform( const SE3 & se3 )
{
	source_se3          = se3;
	inverted_source_se3 = se3.inverse();
}



bool MarchingCubesDual::subdivide_source( Float bounding_radius, VolumeSource * source, const DistanceScalerBase * scaler )
{
	_values_map.clear();
	_normals_map.clear();

    const int root_size_int = find_subdivision_levels( bounding_radius, source );
	const int root_size_int_2 = root_size_int / 2;
	MarchingCubesDualNode * root_node = create_node();
	root_node->size = root_size_int;
	root_node->at   = VectorInt( -root_size_int_2, -root_size_int_2, -root_size_int_2 );

	root_node->subdivide( this, source, scaler );

    //const bool ok = find_surface( source, scaler, surface_node );
    //if (!ok)
    //    return false;

 //   _all_nodes.clear();
 //   _recently_added_nodes.clear();
 //   _new_candidates.clear();
 //   _all_nodes.insert( surface_node );
 //   _recently_added_nodes.insert( surface_node );

	//int nodes_qty = 1;
 //   for ( ;; )
 //   {
 //       for ( MarchingSetIterator it=_recently_added_nodes.begin(); it!=_recently_added_nodes.end(); it++ )
 //       {
 //           const MarchingCubesDualNode & node = *it;
 //           const bool on_surface = node.has_surface( iso_level );
 //           if ( on_surface )
 //               add_node_neighbors( node, source, scaler, nodes_qty );
 //       }
 //       if ( _new_candidates.empty() )
 //           break;
 //       
 //       _recently_added_nodes = _new_candidates;
 //       _new_candidates.clear();

	//	if ( (max_nodes_qty > 0) && (nodes_qty >= max_nodes_qty) )
	//		break;
	//}

	//// Create faces and assign material.
	//_all_faces.clear();
	//_materials.clear();
	//_materials_set.clear();
	//for ( MarchingSetIterator it=_all_nodes.begin(); it!=_all_nodes.end(); it++ )
	//{
	//	const MarchingNode node = *it;
	//	const bool on_surface = node.has_surface( iso_level );
	//	if ( on_surface )
	//	{
	//		const int material_index = node_material( source, node, scaler );

	//		create_faces( node, material_index );
	//		_materials_set.insert( material_index );
	//	}
	//}

	//// Normalize normals.
	//for ( NormalsMapIterator it=_normals_map.begin(); it!=_normals_map.end(); it++ )
	//{
	//	NormalsAndQty & norms = it->second;
	//	const Vector3d norm = norms.norms / static_cast<Float>(norms.qty);
	//	norms.norms = norm;
	//}

	//// Assign vertex positions, normals, and tangents.
	//_verts.clear();
	//_norms.clear();
	//_tangs.clear();
	//const int faces_qty = _all_faces.size();
	//for ( int i=0; i<faces_qty; i++ )
	//{
	//	const NodeFace & face = _all_faces[i];
	//	const Face3 & f = face.face;
	//	const Vector3 norm = f.get_plane().normal;
	//	const Vector3 tangent = norm.cross( f.vertex[2] - f.vertex[0] ).normalized();
	//	for ( int j=0; j<3; j++ )
	//	{
	//		const Vector3 vert          = f.vertex[j];
	//		const NodeEdgeInt & edge    = face.node_edges[j];
	//		NormalsMapConstIterator it  = _normals_map.find( edge );
	//		const NormalsAndQty & n_qty = it->second;
	//		const Vector3d & n          = n_qty.norms;

	//		const Vector3 norm( n.x_, n.y_, n.z_ );

	//		_verts.push_back( vert );
	//		_norms.push_back( norm );
	//		_tangs.push_back( tangent.x );
	//		_tangs.push_back( tangent.y );
	//		_tangs.push_back( tangent.z );
	//		_tangs.push_back( 1.0 );
	//	}
	//}

    return true;
}

bool MarchingCubesDual::should_split( MarchingCubesDualNode * node, VolumeSource * source, const DistanceScalerBase * scaler )
{
	const VectorInt center_int = node->center();
	const Vector3d center = at_in_source_unscaled( center_int, scaler );
	const Float max_sz = source->max_node_size_at( center );
	const Float node_sz = node_size( node->size );
	// If bigger than maximum allowed, subdivide.
	if ( node_sz > max_sz )
		return true;

	// If smaller than minimum allowed, don't subdivide.
	const Float min_sz = source->min_node_size_at( center );
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

	// Compute the distance to the source in these points.
	// And compare with interpolated values.
	compute_node_values( *node, source, scaler );

	// But first check if the node has the surface.
	// If not, there is no point subdividing.
	const bool has_surface = node->has_surface();
	if ( !has_surface )
		return false;

	// For now just the central point.
	Float interpolated_value = 0.0;
	for ( int i=0; i<8; i++ )
		interpolated_value += node->values[i];
	interpolated_value /= 8.0;

	const Vector3d  center_float = at_in_source_unscaled( center_int, scaler );
	const Float actual_value     = value_at( source, center_int, center_float );

	const Float rel_diff = std::abs( actual_value - interpolated_value ) / node_sz;
	// Compare this difference per unit node size with the threshold.
	const bool do_split = ( rel_diff > max_rel_diff );
	return do_split;
}

MarchingCubesDualNode * MarchingCubesDual::create_node()
{
	MarchingCubesDualNode * node = memnew( MarchingCubesDualNode );
	_octree_nodes.push_back( node );
	return node;
}


const std::set<int> & MarchingCubesDual::materials() const
{
	return _materials_set;
}

const std::vector<Vector3> & MarchingCubesDual::vertices( int material_ind )
{
	const unsigned int qty = _materials.size();
	_ret_verts.clear();
	_ret_verts.reserve(3*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const int ind = _materials[i];
		if (ind != material_ind)
			continue;
		for ( int j=0; j<3; j++ )
		{
			const int ind = 3*i + j;
			const Vector3 & v = _verts[ind];
			_ret_verts.push_back( v );
		}
	}
	return _ret_verts;
}

const std::vector<Vector3> & MarchingCubesDual::normals( int material_ind )
{
	const unsigned int qty = _materials.size();
	_ret_norms.clear();
	_ret_norms.reserve(3*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const int ind = _materials[i];
		if (ind != material_ind)
			continue;
		for ( int j=0; j<3; j++ )
		{
			const int ind = 3*i + j;
			const Vector3 & v = _norms[ind];
			_ret_norms.push_back( v );
		}
	}
	return _ret_norms;
}

const std::vector<real_t>  & MarchingCubesDual::tangents( int material_ind )
{
	const unsigned int qty = _materials.size();
	_ret_tangs.clear();
	_ret_verts.reserve(4*qty);
	for ( unsigned int i=0; i<qty; i++ )
	{
		const int ind = _materials[i];
		if (ind != material_ind)
			continue;

		for ( int j=0; j<4; j++ )
		{
			const int ind = 4*i + j;
			const real_t tang = _tangs[ind];
			_ret_tangs.push_back( tang );
		}
	}
	return _ret_tangs;
}

const std::vector<Vector3> & MarchingCubesDual::collision_faces( const Float dist, const DistanceScalerBase * scaler )
{
	const int qty = _all_faces.size();
	const Transform t = source_transform( scaler );
	const Vector3 o = Vector3(0.0, 0.0, 0.0);
	_ret_verts.clear();

	for ( int i=0; i<qty; i++ )
	{
		const NodeFace & nf = _all_faces[i];
		Face3 f = nf.face;
		f.vertex[0] = t.xform( f.vertex[0] );
		f.vertex[1] = t.xform( f.vertex[1] );
		f.vertex[2] = t.xform( f.vertex[2] );
		const Float d = f.get_closest_point_to( o ).length();
		if ( d <= dist )
		{
			_ret_verts.push_back( f.vertex[0] );
			_ret_verts.push_back( f.vertex[1] );
			_ret_verts.push_back( f.vertex[2] );
		}
	}

	return _ret_verts;
}



const Transform MarchingCubesDual::source_transform( const DistanceScalerBase * scaler ) const
{
	Vector3d o;
	if (scaler == nullptr)
		o = source_se3.r_;
	else
		o = scaler->scale( source_se3.r_ );

	SE3 se3( source_se3 );
	se3.r_ = o;
	const Transform ret = se3.transform();
	return ret;
}





Float MarchingCubesDual::node_size( int size ) const
{
    const Float sz = static_cast<Float>( size ) * step;
    return sz;
}


Vector3d MarchingCubesDual::at_in_source( const VectorInt & at_i ) const
{
	const Vector3d at( static_cast<Float>(at_i.x)*step, 
		               static_cast<Float>(at_i.y)*step, 
		               static_cast<Float>(at_i.z)*step );
	return at;
}

Vector3d MarchingCubesDual::at_in_source_unscaled( const VectorInt & at_i, const DistanceScalerBase * scaler ) const
{
    const Vector3d at( static_cast<Float>(at_i.x)*step, 
                       static_cast<Float>(at_i.y)*step, 
                       static_cast<Float>(at_i.z)*step );
    // Apply transform.
    if (scaler == nullptr)
    {
        const Vector3d at_in_source = at;
        return at_in_source;
    }

	const Vector3d origin_in_world_scaled   = scaler->scale( source_se3.r_ );
	const Vector3d at_in_world_scaled       = source_se3.q_ * at + origin_in_world_scaled;
	const Vector3d at_in_world_unscaled     = scaler->unscale( at_in_world_scaled );
	const Vector3d origin_in_world_unscaled = source_se3.r_;
	const Vector3d at_in_world_unscaled_relative_to_origin = at_in_world_unscaled - origin_in_world_unscaled;
	const Vector3d at_in_source_unscaled_ret = inverted_source_se3.q_ * at_in_world_unscaled_relative_to_origin;
	return at_in_source_unscaled_ret;
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
}

int MarchingCubesDual::find_subdivision_levels( Float bounding_radius, VolumeSource * source )
{
	step = source->min_node_size();
	const Float max_sz = bounding_radius * 2.0;

	int size_int = 2;
	Float sz = step;
	while (sz <= max_sz)
	{
		sz *= 2.0;
		size_int += 1;
	}

	return size_int;
}


//bool MarchingCubesDual::find_surface( VolumeSource * source, const DistanceScalerBase * scaler, MarchingNode & surface_node )
//{
//	MarchingCubesDualNode node;
//	compute_node_values( node, source, scaler );
//	for ( int i=0; i<100000; i++ )
//	{
//		const bool has_surface = node.has_surface( iso_level );
//		if ( has_surface )
//		{
//			surface_node = node;
//			return true;
//		}
//		node = step_towards_surface( node, source, scaler );
//	}
//
//	return false;
//}


void MarchingCubesDual::compute_node_values( MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler )
{
    VectorInt verts[8];
    const int x  = node.at.x;
    const int y  = node.at.y;
    const int z  = node.at.z;
    const int sz = node.size;
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
		node.vertices_int[i] = verts[i];
        // Value at unwarped position.
		const VectorInt & vert_int = verts[i];
        const Vector3d vert_d = at_in_source_unscaled( verts[i], scaler );
        const Float    value  = value_at( source, vert_int, vert_d );
		// Store unwarped position.
		node.vertices_unscaled[i] = vert_d;
        // Store warped position.
        const Vector3d vert = at_in_source( verts[i] );
        node.vertices[i]    = vert;
        node.values[i]      = value;
    }
}



void MarchingCubesDual::node_proc( const MarchingCubesDualNode * n )
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

		node_proc( c0 );
		node_proc( c1 );
		node_proc( c2 );
		node_proc( c3 );
		node_proc( c4 );
		node_proc( c5 );
		node_proc( c6 );
		node_proc( c7 );

		face_proc_xy(c0, c3);
		face_proc_xy(c1, c2);
		face_proc_xy(c4, c7);
		face_proc_xy(c5, c6);

		face_proc_zy(c0, c1);
		face_proc_zy(c3, c2);
		face_proc_zy(c4, c5);
		face_proc_zy(c7, c6);

		face_proc_xz(c4, c0);
		face_proc_xz(c5, c1);
		face_proc_xz(c7, c3);
		face_proc_xz(c6, c2);

		edge_proc_x(c0, c3, c7, c4);
		edge_proc_x(c1, c2, c6, c5);

		edge_proc_y(c0, c1, c2, c3);
		edge_proc_y(c4, c5, c6, c7);

		edge_proc_z(c7, c6, c2, c3);
		edge_proc_z(c4, c5, c1, c0);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}


void MarchingCubesDual::face_proc_xy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1 )
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

		face_proc_xy(c0, c3);
		face_proc_xy(c1, c2);
		face_proc_xy(c4, c7);
		face_proc_xy(c5, c6);

		edge_proc_x(c0, c3, c7, c4);
		edge_proc_x(c1, c2, c6, c5);
		edge_proc_y(c0, c1, c2, c3);
		edge_proc_y(c4, c5, c6, c7);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}

void MarchingCubesDual::face_proc_zy( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1 )
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

		face_proc_zy(c0, c1);
		face_proc_zy(c3, c2);
		face_proc_zy(c4, c5);
		face_proc_zy(c7, c6);

		edge_proc_y(c0, c1, c2, c3);
		edge_proc_y(c4, c5, c6, c7);
		edge_proc_z(c7, c6, c2, c3);
		edge_proc_z(c4, c5, c1, c0);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}

void MarchingCubesDual::face_proc_xz( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1 )
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

		face_proc_xz(c4, c0);
		face_proc_xz(c5, c1);
		face_proc_xz(c7, c3);
		face_proc_xz(c6, c2);

		edge_proc_x(c0, c3, c7, c4);
		edge_proc_x(c1, c2, c6, c5);
		edge_proc_z(c7, c6, c2, c3);
		edge_proc_z(c4, c5, c1, c0);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}


void MarchingCubesDual::edge_proc_x( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3 )
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

		edge_proc_x(c0, c3, c7, c4);
		edge_proc_x(c1, c2, c6, c5);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}

void MarchingCubesDual::edge_proc_y( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3 )
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

		edge_proc_y(c0, c1, c2, c3);
		edge_proc_y(c4, c5, c6, c7);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}

void MarchingCubesDual::edge_proc_z( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3 )
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

		edge_proc_z(c7, c6, c2, c3);
		edge_proc_z(c4, c5, c1, c0);

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
}


void MarchingCubesDual::vert_proc( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
	const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7 )
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

		vert_proc(c0, c1, c2, c3, c4, c5, c6, c7);
	}
	else
	{

		//if (!n0->isIsoSurfaceNear() && !n1->isIsoSurfaceNear() && !n2->isIsoSurfaceNear() && !n3->isIsoSurfaceNear() &&
		//	!n4->isIsoSurfaceNear() && !n5->isIsoSurfaceNear() && !n6->isIsoSurfaceNear() && !n7->isIsoSurfaceNear())
		//{
		//	return;
		//}

		//Vector4 values[8];
		//values[0] = n0->getCenterValue();
		//values[1] = n1->getCenterValue();
		//values[2] = n2->getCenterValue();
		//values[3] = n3->getCenterValue();
		//values[4] = n4->getCenterValue();
		//values[5] = n5->getCenterValue();
		//values[6] = n6->getCenterValue();
		//values[7] = n7->getCenterValue();
		//addDualCell(n0->getCenter(), n1->getCenter(), n2->getCenter(), n3->getCenter(),
		//	n4->getCenter(), n5->getCenter(), n6->getCenter(), n7->getCenter(), values);

		create_border_cells(n0, n1, n2, n3, n4, n5, n6, n7);
	}
}


void MarchingCubesDual::create_border_cells( const MarchingCubesDualNode * n0, const MarchingCubesDualNode * n1, const MarchingCubesDualNode * n2, const MarchingCubesDualNode * n3,
										     const MarchingCubesDualNode * n4, const MarchingCubesDualNode * n5, const MarchingCubesDualNode * n6, const MarchingCubesDualNode * n7 )
{
	const MarchingCubesDualNode * root = _octree_nodes[0];

	if (n0->at_back( root ) && n1->at_back( root ) && n4->at_back( root ) && n5->at_back( root ))
	{
		addDualCell(n0->center_back(), n1->center_back(), n1->center(), n0->center(),
			n4->center_back(), n5->center_back(), n5->center(), n4->center());
		// Generate back edge border cells
		if (n4->at_top( root ) && n5->at_top( root ))
		{
			addDualCell(n4->center_back(), n5->center_back(), n5->center(), n4->center(),
				n4->center_back_top(), n5->center_back_top(), n5->center_top(), n4->center_top());
			// Generate back top corner cells
			if (n4->at_left( root ))
			{
				addDualCell(n4->center_back_left(), n4->center_back(), n4->center(), n4->center_left(),
					n4->corner_4(), n4->center_back_top(), n4->center_top(), n4->center_left_top());
			}
			if (n5->at_right( root ))
			{
				addDualCell(n5->center_back(), n5->center_back_right(), n5->center_right(), n5->center(),
					n5->center_back_top(), n5->corner_5(), n5->center_right_top(), n5->center_top());
			}
		}
		if (n0->at_bottom( root ) && n1->at_bottom( root ))
		{
			addDualCell(n0->center_back_bottom(), n1->center_back_bottom(), n1->center_bottom(), n0->center_bottom(),
				n0->center_back(), n1->center_back(), n1->center(), n0->center());
			// Generate back bottom corner cells
			if (n0->at_left( root ))
			{
				addDualCell(n0->corner_0(), n0->center_back_bottom(), n0->center_bottom(), n0->center_left_bottom(),
					n0->center_back_left(), n0->center_back(), n0->center(), n0->center_left());
			}
			if (n1->at_right( root ))
			{
				addDualCell(n1->center_back_bottom(), n1->corner_1(), n1->center_right_bottom(), n1->center_bottom(),
					n1->center_back(), n1->center_back_right(), n1->center_right(), n1->center());
			}
		}
	}
	if (n2->at_front( root ) && n3->at_front( root ) && n6->at_front( root ) && n7->at_front( root ))
	{
		addDualCell(n3->center(), n2->center(), n2->center_front(), n3->center_front(),
			n7->center(), n6->center(), n6->center_front(), n7->center_front());
		// Generate front edge border cells
		if (n6->at_top( root ) && n7->at_top( root ))
		{
			addDualCell(n7->center(), n6->center(), n6->center_front(), n7->center_front(),
				n7->center_top(), n6->center_top(), n6->center_front_top(), n7->center_front_top());
			// Generate back bottom corner cells
			if (n7->at_left( root ))
			{
				addDualCell(n7->center_left(), n7->center(), n7->center_front(), n7->center_front_left(),
					n7->center_left_top(), n7->center_top(), n7->center_front_top(), n7->corner_7());
			}
			if (n6->at_right( root ))
			{
				addDualCell(n6->center(), n6->center_right(), n6->center_front_right(), n6->center_front(),
					n6->center_top(), n6->center_right_top(), n6->corner_6(), n6->center_front_top());
			}
		}
		if (n3->at_bottom( root ) && n2->at_bottom( root ))
		{
			addDualCell(n3->center_bottom(), n2->center_bottom(), n2->center_front_bottom(), n3->center_front_bottom(), 
				n3->center(), n2->center(), n2->center_front(), n3->center_front());
			// Generate back bottom corner cells
			if (n3->at_left( root ))
			{
				addDualCell(n3->center_left_bottom(), n3->center_bottom(), n3->center_front_bottom(), n3->corner_3(),
					n3->center_left(), n3->center(), n3->center_front(), n3->center_front_left());
			}
			if (n2->at_right( root ))
			{
				addDualCell(n2->center_bottom(), n2->center_right_bottom(), n2->corner_2(), n2->center_front_bottom(),
					n2->center(), n2->center_right(), n2->center_front_right(), n2->center_front());
			}
		}
	}
	if (n0->at_left( root ) && n3->at_left( root ) && n4->at_left( root ) && n7->at_left( root ))
	{
		addDualCell(n0->center_left(), n0->center(), n3->center(), n3->center_left(),
			n4->center_left(), n4->center(), n7->center(), n7->center_left());
		// Generate left edge border cells
		if (n4->at_top( root ) && n7->at_top( root ))
		{
			addDualCell(n4->center_left(), n4->center(), n7->center(), n7->center_left(),
				n4->center_left_top(), n4->center_top(), n7->center_top(), n7->center_left_top());
		}
		if (n0->at_bottom( root ) && n3->at_bottom( root ))
		{
			addDualCell(n0->center_left_bottom(), n0->center_bottom(), n3->center_bottom(), n3->center_left_bottom(),
				n0->center_left(), n0->center(), n3->center(), n3->center_left());
		}
		if (n0->at_back( root ) && n4->at_back( root ))
		{
			addDualCell(n0->center_back_left(), n0->center_back(), n0->center(), n0->center_left(),
				n4->center_back_left(), n4->center_back(), n4->center(), n4->center_left());
		}
		if (n3->at_front( root ) && n7->at_front( root ))
		{
			addDualCell(n3->center_left(), n3->center(), n3->center_front(), n3->center_front_left(),
				n7->center_left(), n7->center(), n7->center_front(), n7->center_front_left());
		}
	}
	if (n1->at_right( root ) && n2->at_right( root ) && n5->at_right( root ) && n6->at_right( root ))
	{
		addDualCell(n1->center(), n1->center_right(), n2->center_right(), n2->center(),
			n5->center(), n5->center_right(), n6->center_right(), n6->center());
		// Generate right edge border cells
		if (n5->at_top( root ) && n6->at_top( root ))
		{
			addDualCell(n5->center(), n5->center_right(), n6->center_right(), n6->center(),
				n5->center_top(), n5->center_right_top(), n6->center_right_top(), n6->center_top());
		}
		if (n1->at_bottom( root ) && n2->at_bottom( root ))
		{
			addDualCell(n1->center_bottom(), n1->center_right_bottom(), n2->center_right_bottom(), n2->center_bottom(),
				n1->center(), n1->center_right(), n2->center_right(), n2->center());
		}
		if (n1->at_back( root ) && n5->at_back( root ))
		{
			addDualCell(n1->center_back(), n1->center_back_right(), n1->center_right(), n1->center(),
				n5->center_back(), n5->center_back_right(), n5->center_right(), n5->center());
		}
		if (n2->at_front( root ) && n6->at_front( root ))
		{
			addDualCell(n2->center(), n2->center_right(), n2->center_front_right(), n2->center_front(),
				n6->center(), n6->center_right(), n6->center_front_right(), n6->center_front());
		}
	}
	if (n4->at_top( root ) && n5->at_top( root ) && n6->at_top( root ) && n7->at_top( root ))
	{
		addDualCell(n4->center(), n5->center(), n6->center(), n7->center(),
			n4->center_top(), n5->center_top(), n6->center_top(), n7->center_top());
	}
	if (n0->at_bottom( root ) && n1->at_bottom( root ) && n2->at_bottom( root ) && n3->at_bottom( root ))
	{
		addDualCell(n0->center_bottom(), n1->center_bottom(), n2->center_bottom(), n3->center_bottom(),
			n0->center(), n1->center(), n2->center(), n3->center());
	}

}








//MarchingNode MarchingCubesDual::step_towards_surface( const MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler )
//{
//	const Float center = node.values[1] + node.values[2] + node.values[6] + node.values[5] + 
//					     node.values[0] + node.values[3] + node.values[7] + node.values[4];
//	const Float sign = (center >= 0.0) ? 1.0 : -1.0;
//    const Float dx = ( (node.values[1] + node.values[2] + node.values[6] + node.values[5]) - 
//                       (node.values[0] + node.values[3] + node.values[7] + node.values[4]) ) * sign;
//    const Float dy = ( (node.values[4] + node.values[5] + node.values[6] + node.values[7]) - 
//                       (node.values[0] + node.values[1] + node.values[2] + node.values[3]) ) * sign;
//    const Float dz = ( (node.values[3] + node.values[7] + node.values[6] + node.values[2]) - 
//                       (node.values[0] + node.values[4] + node.values[5] + node.values[1]) ) * sign;
//    const Float abs_vals[3] = { std::abs(dx), std::abs(dy), std::abs(dz) };
//    Float max_val = -1.0;
//    int   max_ind = -1;
//    for ( int i=0; i<3; i++ )
//    {
//        const int val = abs_vals[i];
//        if ( (max_ind < 0) || (max_val < val) )
//        {
//            max_ind = i;
//            max_val = val;
//        }
//    }
//
//	MarchingCubesDualNode next_node( node );
//    if ( max_ind == 0 )
//    {
//        if (dx > 0.0)
//            next_node.at.x -= node.size;
//        else
//            next_node.at.x += node.size;
//    }
//    else if ( max_ind == 1 )
//    {
//        if (dy > 0.0)
//            next_node.at.y -= node.size;
//        else
//            next_node.at.y += node.size;
//    }
//    else
//    {
//        if (dz > 0.0)
//            next_node.at.z -= node.size;
//        else
//            next_node.at.z += node.size;
//    }
//
//    // Compute node values.
//    compute_node_values( next_node, source, scaler );
//
//    // Actually, as "value" is the distance, 
//    // I might take a more correct adjustment in order to 
//    // reduce number of steps needed to get a node containing the surface.
//
//    return next_node;
//}


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


//void MarchingCubesDual::add_node_neighbors( const MarchingCubesDualNode & node, VolumeSource * source, const DistanceScalerBase * scaler, int & nodes_qty )
//{
//
//    for ( int ix=0; ix<3; ix++ )
//    {
//        const int x = node.at.x + node.size * (ix-1);
//        for ( int iy=0; iy<3; iy++ )
//        {
//            const int y = node.at.y + node.size * (iy-1);
//            for ( int iz=0; iz<3; iz++ )
//            {
//                if ( (ix==1) && (iy==1) && (iz==1) )
//                    continue;
//                const int z = node.at.z + node.size * (iz-1);
//                MarchingNode candidate;
//                candidate.at = VectorInt( x, y, z );
//                candidate.size = node.size;
//
//                compute_node_values( candidate, source, scaler );
//
//				MarchingSetConstIterator it = _all_nodes.find( candidate );
//                if ( it == _all_nodes.end() )
//                {
//					/*{
//						for ( MarchingSetConstIterator it2=_all_nodes.begin(); it2!=_all_nodes.end(); it2++ )
//						{
//							const MarchingNode & n = *it2;
//							const bool equal = (n == candidate);
//							if ( equal )
//							{
//								const bool equal2 = (n == candidate);
//								MarchingSetConstIterator it3 = _all_nodes.find( candidate );
//								int iii = 0;
//							}
//						}
//					}*/
//                    _all_nodes.insert( candidate );
//                    _new_candidates.insert( candidate );
//					nodes_qty += 1;
//                }
//            }
//        }
//    }
//}


void MarchingCubesDual::create_faces( const MarchingCubesDualNode & node, int material_index )
{
	uint32_t cube_index = 0;
	for ( int i=0; i<8; i++ )
	{
		const Float value = node.values[i];
		if ( value >= iso_level )
			cube_index |= (1 << i);
	}
	const int edge = CubeTables::EDGES[cube_index];
	Vector3d intersection_points[12];
	NodeEdgeInt edges_int[12];
	if ( edge & 1 )
	{
		intersection_points[0]  = interpolate( node.vertices[0], node.vertices[1], node.values[0], node.values[1] );
		edges_int[0] = NodeEdgeInt( node.vertices_int[0], node.vertices_int[1] );
	}
	if ( edge & 2 )
	{
		intersection_points[1]  = interpolate( node.vertices[1], node.vertices[2], node.values[1], node.values[2] );
		edges_int[1] = NodeEdgeInt( node.vertices_int[1], node.vertices_int[2] );
	}
	if ( edge & 4 )
	{
		intersection_points[2]  = interpolate( node.vertices[2], node.vertices[3], node.values[2], node.values[3] );
		edges_int[2] = NodeEdgeInt( node.vertices_int[2], node.vertices_int[3] );
	}
	if ( edge & 8 )
	{
		intersection_points[3]  = interpolate( node.vertices[3], node.vertices[0], node.values[3], node.values[0] );
		edges_int[3] = NodeEdgeInt( node.vertices_int[3], node.vertices_int[0] );
	}
	if ( edge & 16 )
	{
		intersection_points[4]  = interpolate( node.vertices[4], node.vertices[5], node.values[4], node.values[5] );
		edges_int[4] = NodeEdgeInt( node.vertices_int[4], node.vertices_int[5] );
	}
	if ( edge & 32 )
	{
		intersection_points[5]  = interpolate( node.vertices[5], node.vertices[6], node.values[5], node.values[6] );
		edges_int[5] = NodeEdgeInt( node.vertices_int[5], node.vertices_int[6] );
	}
	if ( edge & 64 )
	{
		intersection_points[6]  = interpolate( node.vertices[6], node.vertices[7], node.values[6], node.values[7] );
		edges_int[6] = NodeEdgeInt( node.vertices_int[6], node.vertices_int[7] );
	}
	if ( edge & 128 )
	{
		intersection_points[7]  = interpolate( node.vertices[7], node.vertices[4], node.values[7], node.values[4] );
		edges_int[7] = NodeEdgeInt( node.vertices_int[7], node.vertices_int[4] );
	}
	if ( edge & 256 )
	{
		intersection_points[8]  = interpolate( node.vertices[0], node.vertices[4], node.values[0], node.values[4] );
		edges_int[8] = NodeEdgeInt( node.vertices_int[0], node.vertices_int[4] );
	}
	if ( edge & 512 )
	{
		intersection_points[9]  = interpolate( node.vertices[1], node.vertices[5], node.values[1], node.values[5] );
		edges_int[9] = NodeEdgeInt( node.vertices_int[1], node.vertices_int[5] );
	}
	if ( edge & 1024 )
	{
		intersection_points[10] = interpolate( node.vertices[2], node.vertices[6], node.values[2], node.values[6] );
		edges_int[10] = NodeEdgeInt( node.vertices_int[2], node.vertices_int[6] );
	}
	if ( edge & 2048 )
	{
		intersection_points[11] = interpolate( node.vertices[3], node.vertices[7], node.values[3], node.values[7] );
		edges_int[11] = NodeEdgeInt( node.vertices_int[3], node.vertices_int[7] );
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

		const Vector3 fa( a.x_, a.y_, a.z_ );
		const Vector3 fb( b.x_, b.y_, b.z_ );
		const Vector3 fc( c.x_, c.y_, c.z_ );
		const Face3 f( fa, fb, fc );

		const NodeFace face( f, edge_a, edge_b, edge_c );
		_all_faces.push_back( face );
		_materials.push_back( material_index );

		const Vector3 norm = f.get_plane().normal;
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

int MarchingCubesDual::node_material( VolumeSource * source, const MarchingCubesDualNode & node, const DistanceScalerBase * scaler )
{
	int max_priority = -1;
	int max_material = -1;
	for ( int i=0; i<8; i++ )
	{
		const VectorInt & at_i = node.vertices_int[i];
		const Vector3d    at_d = at_in_source_unscaled( at_i, scaler );
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




}





