
#include "marching_cubes.h"
#include "volume_source.h"
#include "cube_tables.h"
#include "vector3d.h"
#include "distance_scaler.h"

#include <cmath>


namespace Ign
{


MarchingCubes::MarchingCubes()
{
	iso_level  = 0.0;
	eps        = 1.0e-4;
	step       = 1.0;
	levels_qty = 1;
}


MarchingCubes::~MarchingCubes()
{}


void MarchingCubes::set_source_transform( const SE3 & se3 )
{
	source_se3 = se3;
}



bool MarchingCubes::subdivide_source( VolumeSource * source, DistanceScaler * scaler )
{
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

    for ( ;; )
    {
        for ( MarchingSetIterator it=_recently_added_nodes.begin(); it!=_recently_added_nodes.end(); it++ )
        {
            const MarchingNode & node = *it;
            const bool on_surface = node.has_surface();
            if ( on_surface )
                add_node_neighbors( node, source, scaler );
        }
        if ( _new_candidates.empty() )
            break;
        
        _recently_added_nodes = _new_candidates;
        _new_candidates.clear();
    }

	_all_faces.clear();
	for ( MarchingSetIterator it=_all_nodes.begin(); it!=_all_nodes.end(); it++ )
	{
		const MarchingNode node = *it;
		create_faces( node );
	}

    return true;
}


Float MarchingCubes::node_size( int level ) const
{
    const Float sz = std::pow( 2.0, level ) * step;
    return sz;
}


Vector3d MarchingCubes::at( const VectorInt & at_i, DistanceScaler * scaler ) const
{
    const Vector3d at( static_cast<Float>(at_i.x)*step, 
                       static_cast<Float>(at_i.y)*step, 
                       static_cast<Float>(at_i.z)*step );
    // Apply transform.
    if (scaler == nullptr)
    {
        const Vector3d at_in_source = source_se3.q_ * at + source_se3.r_;
        return at_in_source;
    }

    const Vector3d at_real      = scaler->unscale( at );
    const Vector3d at_in_source = source_se3.q_ * at_real + source_se3.r_;
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


bool MarchingCubes::find_surface( VolumeSource * source, DistanceScaler * scaler, MarchingNode & surface_node )
{
	MarchingNode node;
	for ( int i=0; i<100000; i++ )
	{
		compute_node_values( node, source, scaler );
		const bool has_surface = node.has_surface();
		if ( has_surface )
		{
			surface_node = node;
			return true;
		}
	}

	return false;
}


void MarchingCubes::compute_node_values( MarchingNode & node, VolumeSource * source, DistanceScaler * scaler ) const
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
    verts[7] = VectorInt( x     , y + sz, z + sz );

    for ( int i=0; i<8; i++ )
    {
		node.vertices_int[i] = verts[i];
        // Value at warped position.
        const Vector3d vert_d = at( verts[i], scaler );
        const Float    value  = source->value( vert_d );
        // Store unwarped position.
        const Vector3d vert   = at( verts[i], nullptr );
        node.vertices[i] = vert;
        node.values[i]   = value;
    }
}


MarchingNode MarchingCubes::step_towards_surface( const MarchingNode & node, VolumeSource * source, DistanceScaler * scaler ) const
{
    const Float dx = (node.values[1] + node.values[2] + node.values[6] + node.values[5]) - 
                     (node.values[0] + node.values[3] + node.values[7] + node.values[4]);
    const Float dy = (node.values[4] + node.values[5] + node.values[6] + node.values[7]) - 
                     (node.values[0] + node.values[1] + node.values[2] + node.values[3]);
    const Float dz = (node.values[3] + node.values[7] + node.values[6] + node.values[2]) - 
                     (node.values[0] + node.values[4] + node.values[5] + node.values[1]);
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
            next_node.at.x += node.size;
        else
            next_node.at.x -= node.size;
    }
    else if ( max_ind == 1 )
    {
        if (dy > 0.0)
            next_node.at.y += node.size;
        else
            next_node.at.y -= node.size;
    }
    else
    {
        if (dz > 0.0)
            next_node.at.z += node.size;
        else
            next_node.at.z -= node.size;
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


void MarchingCubes::add_node_neighbors( const MarchingNode & node, VolumeSource * source, DistanceScaler * scaler )
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

                std::set<MarchingNode, MarchingNodeCompare>::iterator it = _all_nodes.find( candidate );
                if ( it != _all_nodes.end() )
                {
                    _all_nodes.insert( candidate );
                    _new_candidates.insert( candidate );
                }
            }
        }
    }
}


void MarchingCubes::create_faces( const MarchingNode & node )
{
	uint32_t cube_index = 0;
	for ( int i=0; i<8; i++ )
	{
		if ( node.values[i] >= iso_level )
			cube_index |= (1 << i);
	}
	const int edge = CubeTables::EDGES[cube_index];
	Vector3d intersection_points[12];
	if ( edge & 1 )
		intersection_points[0]  = interpolate( node.vertices[0], node.vertices[1], node.values[0], node.values[1] );
	if ( edge & 2 )
		intersection_points[1]  = interpolate( node.vertices[1], node.vertices[2], node.values[1], node.values[2] );
	if ( edge & 4 )
		intersection_points[2]  = interpolate( node.vertices[2], node.vertices[3], node.values[2], node.values[3] );
	if ( edge & 8 )
		intersection_points[3]  = interpolate( node.vertices[3], node.vertices[0], node.values[3], node.values[0] );
	if ( edge & 16 )
		intersection_points[4]  = interpolate( node.vertices[4], node.vertices[5], node.values[4], node.values[5] );
	if ( edge & 32 )
		intersection_points[5]  = interpolate( node.vertices[5], node.vertices[6], node.values[5], node.values[6] );
	if ( edge & 64 )
		intersection_points[4]  = interpolate( node.vertices[6], node.vertices[7], node.values[6], node.values[7] );
	if ( edge & 128 )
		intersection_points[7]  = interpolate( node.vertices[7], node.vertices[4], node.values[7], node.values[4] );
	if ( edge & 256 )
		intersection_points[8]  = interpolate( node.vertices[0], node.vertices[4], node.values[0], node.values[4] );
	if ( edge & 512 )
		intersection_points[9]  = interpolate( node.vertices[1], node.vertices[5], node.values[1], node.values[5] );
	if ( edge & 1024 )
		intersection_points[10] = interpolate( node.vertices[2], node.vertices[6], node.values[2], node.values[6] );
	if ( edge & 2048 )
		intersection_points[11] = interpolate( node.vertices[3], node.vertices[7], node.values[3], node.values[7] );

	for ( int i=0; CubeTables::TRIANGLES[cube_index][i] != -1; i+=3 )
	{
		const int ind_a = CubeTables::TRIANGLES[cube_index][i];
		const int ind_b = CubeTables::TRIANGLES[cube_index][i+1];
		const int ind_c = CubeTables::TRIANGLES[cube_index][i+2];
		const Vector3d a = intersection_points[ind_a];
		const Vector3d b = intersection_points[ind_b];
		const Vector3d c = intersection_points[ind_c];

		const Vector3 fa( a.x_, a.y_, a.z_ );
		const Vector3 fb( b.x_, b.y_, b.z_ );
		const Vector3 fc( c.x_, c.y_, c.z_ );
		const Face3 f( fa, fb, fc );
		_all_faces.push_back( f );
	}
}


}





