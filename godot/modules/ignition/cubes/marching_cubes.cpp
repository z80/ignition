
#include "marching_cubes.h"
#include "volume_source.h"
#include "material_source.h"
#include "cube_tables.h"
#include "vector3d.h"
#include "distance_scaler.h"

#include <cmath>


namespace Ign
{


MarchingCubes::MarchingCubes()
{
	iso_level     = 0.0;
	eps           = 1.0e-4;
	step          = 1.0;
	levels_qty    = 1;
	max_nodes_qty = -1;
}


MarchingCubes::~MarchingCubes()
{}


void MarchingCubes::set_source_transform( const SE3 & se3 )
{
	source_se3          = se3;
	inverted_source_se3 = se3.inverse();
}



bool MarchingCubes::subdivide_source( VolumeSource * source, const DistanceScaler * scaler )
{
	_values_map.clear();
	_normals_map.clear();

    find_subdivision_levels( source );
    MarchingNode     surface_node;

    const bool ok = find_surface( source, scaler, surface_node );
    if (!ok)
        return false;

    _all_nodes.clear();
    _recently_added_nodes.clear();
    _new_candidates.clear();
    _all_nodes.insert( surface_node );
    _recently_added_nodes.insert( surface_node );

	int nodes_qty = 1;
    for ( ;; )
    {
        for ( MarchingSetIterator it=_recently_added_nodes.begin(); it!=_recently_added_nodes.end(); it++ )
        {
            const MarchingNode & node = *it;
            const bool on_surface = node.has_surface( iso_level );
            if ( on_surface )
                add_node_neighbors( node, source, scaler, nodes_qty );
        }
        if ( _new_candidates.empty() )
            break;
        
        _recently_added_nodes = _new_candidates;
        _new_candidates.clear();

		if ( (max_nodes_qty > 0) && (nodes_qty >= max_nodes_qty) )
			break;
	}

	// Create faces and assign material.
	_all_faces.clear();
	_materials.clear();
	_materials_set.clear();
	for ( MarchingSetIterator it=_all_nodes.begin(); it!=_all_nodes.end(); it++ )
	{
		const MarchingNode node = *it;
		const bool on_surface = node.has_surface( iso_level );
		if ( on_surface )
		{
			const Vector3d center_at = node_center( node, scaler );
			const int material_index = material_at( source, node.at, center_at );

			create_faces( node, material_index );
			_materials_set.insert( material_index );
		}
	}

	// Normalize normals.
	for ( NormalsMapIterator it=_normals_map.begin(); it!=_normals_map.end(); it++ )
	{
		NormalsAndQty & norms = it->second;
		const Vector3d norm = norms.norms / static_cast<Float>(norms.qty);
		norms.norms = norm;
	}

	// Assign vertex positions, normals, and tangents.
	_verts.clear();
	_norms.clear();
	_tangs.clear();
	const int faces_qty = _all_faces.size();
	for ( int i=0; i<faces_qty; i++ )
	{
		const NodeFace & face = _all_faces[i];
		const Face3 & f = face.face;
		const Vector3 norm = f.get_plane().normal;
		const Vector3 tangent = norm.cross( f.vertex[2] - f.vertex[0] ).normalized();
		for ( int j=0; j<3; j++ )
		{
			const Vector3 vert          = f.vertex[j];
			const NodeEdgeInt & edge    = face.node_edges[j];
			NormalsMapConstIterator it  = _normals_map.find( edge );
			const NormalsAndQty & n_qty = it->second;
			const Vector3d & n          = n_qty.norms;

			const Vector3 norm( n.x_, n.y_, n.z_ );

			_verts.push_back( vert );
			_norms.push_back( norm );
			_tangs.push_back( tangent.x );
			_tangs.push_back( tangent.y );
			_tangs.push_back( tangent.z );
			_tangs.push_back( 1.0 );
		}
	}

    return true;
}

const std::set<int> & MarchingCubes::materials() const
{
	return _materials_set;
}

const std::vector<Vector3> & MarchingCubes::vertices( int material_ind )
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

const std::vector<Vector3> & MarchingCubes::normals( int material_ind )
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

const std::vector<real_t>  & MarchingCubes::tangents( int material_ind )
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



const Transform MarchingCubes::source_transform( const DistanceScaler * scaler ) const
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





Float MarchingCubes::node_size( int level ) const
{
    const Float sz = std::pow( 2.0, level ) * step;
    return sz;
}


Vector3d MarchingCubes::at( const VectorInt & at_i, const DistanceScaler * scaler ) const
{
    const Vector3d at( static_cast<Float>(at_i.x)*step, 
                       static_cast<Float>(at_i.y)*step, 
                       static_cast<Float>(at_i.z)*step );
    // Apply transform.
    if (scaler == nullptr)
    {
        const Vector3d at_in_source = inverted_source_se3.q_ * at + inverted_source_se3.r_;
        return at_in_source;
    }

    const Vector3d at_in_source = scaler->unscale( at );
	const Vector3d at_scaled    = inverted_source_se3.q_ * at_in_source + inverted_source_se3.r_;
	return at_in_source;
}

Vector3d MarchingCubes::node_center( const MarchingNode & node, const DistanceScaler * scaler ) const
{
	const VectorInt & at_i = node.at;
	const Vector3d at( (static_cast<Float>(at_i.x) + 0.5)*step, 
          		       (static_cast<Float>(at_i.y) + 0.5)*step, 
		               (static_cast<Float>(at_i.z) + 0.5)*step );
	if (scaler == nullptr)
	{
		const Vector3d at_in_source = inverted_source_se3.q_ * at + inverted_source_se3.r_;
		return at_in_source;
	}

	const Vector3d at_in_source = scaler->unscale( at );
	const Vector3d at_scaled    = inverted_source_se3.q_ * at_in_source + inverted_source_se3.r_;
	return at_in_source;
}

void MarchingCubes::find_subdivision_levels( VolumeSource * source )
{
	step = source->min_node_size();
	const Float max_sz = source->max_node_size();

	levels_qty = 1;
	Float sz = step;
	while (sz < max_sz)
	{
		sz *= 2.0;
		levels_qty += 1;
	}
}


bool MarchingCubes::find_surface( VolumeSource * source, const DistanceScaler * scaler, MarchingNode & surface_node )
{
	MarchingNode node;
	compute_node_values( node, source, scaler );
	for ( int i=0; i<100000; i++ )
	{
		const bool has_surface = node.has_surface( iso_level );
		if ( has_surface )
		{
			surface_node = node;
			return true;
		}
		node = step_towards_surface( node, source, scaler );
	}

	return false;
}


void MarchingCubes::compute_node_values( MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler )
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
        // Value at warped position.
		const VectorInt & vert_int = verts[i];
        const Vector3d vert_d = at( verts[i], scaler );
        const Float    value  = value_at( source, vert_int, vert_d );
        // Store unwarped position.
        const Vector3d vert = at( verts[i], nullptr );
        node.vertices[i]    = vert;
        node.values[i]      = value;
    }
}


MarchingNode MarchingCubes::step_towards_surface( const MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler )
{
	const Float center = node.values[1] + node.values[2] + node.values[6] + node.values[5] + 
					     node.values[0] + node.values[3] + node.values[7] + node.values[4];
	const Float sign = (center >= 0.0) ? 1.0 : -1.0;
    const Float dx = ( (node.values[1] + node.values[2] + node.values[6] + node.values[5]) - 
                       (node.values[0] + node.values[3] + node.values[7] + node.values[4]) ) * sign;
    const Float dy = ( (node.values[4] + node.values[5] + node.values[6] + node.values[7]) - 
                       (node.values[0] + node.values[1] + node.values[2] + node.values[3]) ) * sign;
    const Float dz = ( (node.values[3] + node.values[7] + node.values[6] + node.values[2]) - 
                       (node.values[0] + node.values[4] + node.values[5] + node.values[1]) ) * sign;
    const Float abs_vals[3] = { std::abs(dx), std::abs(dy), std::abs(dz) };
    Float max_val = -1.0;
    int   max_ind = -1;
    for ( int i=0; i<3; i++ )
    {
        const int val = abs_vals[i];
        if ( (max_ind < 0) || (max_val < val) )
        {
            max_ind = i;
            max_val = val;
        }
    }

    MarchingNode next_node( node );
    if ( max_ind == 0 )
    {
        if (dx > 0.0)
            next_node.at.x -= node.size;
        else
            next_node.at.x += node.size;
    }
    else if ( max_ind == 1 )
    {
        if (dy > 0.0)
            next_node.at.y -= node.size;
        else
            next_node.at.y += node.size;
    }
    else
    {
        if (dz > 0.0)
            next_node.at.z -= node.size;
        else
            next_node.at.z += node.size;
    }

    // Compute node values.
    compute_node_values( next_node, source, scaler );

    // Actually, as "value" is the distance, 
    // I might take a more correct adjustment in order to 
    // reduce number of steps needed to get a node containing the surface.

    return next_node;
}


Vector3d MarchingCubes::interpolate( const Vector3d & v0, const Vector3d & v1, const Float val0, const Float val1 ) const
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


void MarchingCubes::add_node_neighbors( const MarchingNode & node, VolumeSource * source, const DistanceScaler * scaler, int & nodes_qty )
{

    for ( int ix=0; ix<3; ix++ )
    {
        const int x = node.at.x + node.size * (ix-1);
        for ( int iy=0; iy<3; iy++ )
        {
            const int y = node.at.y + node.size * (iy-1);
            for ( int iz=0; iz<3; iz++ )
            {
                if ( (ix==1) && (iy==1) && (iz==1) )
                    continue;
                const int z = node.at.z + node.size * (iz-1);
                MarchingNode candidate;
                candidate.at = VectorInt( x, y, z );
                candidate.size = node.size;

                compute_node_values( candidate, source, scaler );

				MarchingSetConstIterator it = _all_nodes.find( candidate );
                if ( it == _all_nodes.end() )
                {
					/*{
						for ( MarchingSetConstIterator it2=_all_nodes.begin(); it2!=_all_nodes.end(); it2++ )
						{
							const MarchingNode & n = *it2;
							const bool equal = (n == candidate);
							if ( equal )
							{
								const bool equal2 = (n == candidate);
								MarchingSetConstIterator it3 = _all_nodes.find( candidate );
								int iii = 0;
							}
						}
					}*/
                    _all_nodes.insert( candidate );
                    _new_candidates.insert( candidate );
					nodes_qty += 1;
                }
            }
        }
    }
}


void MarchingCubes::create_faces( const MarchingNode & node, int material_index )
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



Float MarchingCubes::value_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at )
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

int MarchingCubes::node_material( VolumeSource* source, const MarchingNode & node, const DistanceScaler * scaler )
{
	int max_obj_ind = -1;
	for ( int i=0; i<8; i++ )
	{
		const VectorInt & at_i = node.vertices_int[i];
		const Vector3d    at_d = at( at_i, scaler );
		const int mat_ind_i = material_at( source, at_i, at );
	}
}


int MarchingCubes::material_at( VolumeSource * source, const VectorInt & vector_int, const Vector3d & at )
{
	MaterialsMapConstIterator it = _materials_map.find( vector_int );
	if ( it != _materials_map.end() )
	{
		const Float v = it->second;
		return v;
	}

	const Float v = source->material_global( at );
	_materials_map[vector_int] = v;

	return v;
}




void MarchingCubes::append_normal( const NodeEdgeInt & edge, const Vector3d & n )
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





