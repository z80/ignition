
#include "marching_cubes.h"

namespace Ign
{


MarchingCubes::MarchingCubes()
{}

MarchingCubes::~MarchingCubes()
{}

void MarchingCubes::set_source_transform( const SE3 & se3 )
{
    source_transform = se3;
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
    MarchingNodeInt node;
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

bool MarchingCubes::subdivide_source( VolumeSource * source, DistanceScaler * scaler )
{
    find_subdivision_levels( source, scaler );
    MarchingNodeInt surface_node;
    const bool ok = find_surface( source, scaler, surface_node );
    if (!ok)
        return false;

    _all_nodes.clear();
    _recently_added_nodes.clear();
    _new_candidates.clear();
    _all_nodes.insert( surface_node );
    _recently_added_nodes.push_back( surface_node );

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

    return true;
}

Float MarchingCubes::node_size( int level ) const
{
    const Float sz = math:pow( 2.0, level ) * step;
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
        const Vector3d at_in_source = se3.q * at_d + se3.r;
        return at_in_source;
    }

    const Vector3d at_real = scaler->unscale( at );
    const Vector3d at_in_source = se3.q * at_real + se3.r;
    return at_in_source;
}


void MarchingCubes::compute_node_values( MarchingNode & node, VolumeSource * source, DistanceScaler * scaler ) const
{
    VectorInt verts[8];
    const int x  = node_int.at.x;
    const int y  = node_int.at.y;
    const int z  = node_int.at.z;
    const int sz = node_int.size;
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
    const Float abs_vals[3] = { std::abs(dx), std::abs(dy, std::abs(dz) };
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

    MarchingNodeInt next_node( node_int );
    if ( max_ind == 0 )
    {
        if (dx > 0.0)
            next_node.at.x += node_int.size;
        else
            next_node.at.x -= node_int.size;
    }
    else if ( max_ind == 1 )
    {
        if (dy > 0.0)
            next_node.at.y += node_int.size;
        else
            next_node.at.y -= node_int.size;
    }
    else
    {
        if (dz > 0.0)
            next_node.at.z += node_int.size;
        else
            next_node.at.z -= node_int.size;
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


}





