
#include "vol_mc.h"
#include "vol_mc_node.h"
#include "vol_geometry.h"
#include "vol_source.h"

#include "vol_mc_tables.h"

namespace Ign
{

VolMc::VolMc()
	: _scale( 0.1 ),
	  _subdivisions( 2 )
{
}

VolMc::~VolMc()
{
}

bool VolMc::build_surface( const VolGeometry * geometry, const VolSource * source, Float scale, Integer subdivisions )
{
	_scale = scale;
	_subdivisions = subdivisions;

	_faces.clear();
	_compute_scale();

	_traverse_nodes_brute_force( geometry, source );
	return true;
}

const Vector<VolFace> & VolMc::faces() const
{
	return _faces;
}

void VolMc::_compute_scale()
{
	// Minimum node size = 2.
	// It is not 1 because I want it to be able to contain the origin.
	// But integer node of size 1 cannot contain origin.
	Integer size = 2;
	for ( Integer i=0; i<_subdivisions; i++ )
		size *= 2;

	_size = _scale * static_cast<Float>(size);
	_size_2 = size / 2;
}

void VolMc::_search_surface_bfs()
{
	VolMcNode whole_volume_node;
	whole_volume_node.at   = VolVectorInt( -_size_2, -_size_2, -_size_2 );
	whole_volume_node.size = _size_2 * 2;
}

void VolMc::_traverse_nodes_brute_force( const VolGeometry * geometry, const VolSource * source )
{
	Vector3d intersection_pts[12];
	for ( int ix=-_size_2; ix<_size_2; ix+=2 )
	{
		for ( int iy=-_size_2; iy<_size_2; iy+=2 )
		{
			for ( int iz=-_size_2; iz<_size_2; iz+=2 )
			{
				VolMcNode node;
				node.at   = VolVectorInt( ix, iy, iz );
				node.size = 2;
				_compute_node_values( node, intersection_pts, geometry, source );
				_create_faces( node, intersection_pts );
			}
		}
	}
}

void VolMc::_compute_node_values( VolMcNode & node, Vector3d * intersection_pts, const VolGeometry * geometry, const VolSource * source )
{
	VolVectorInt verts[8];
	const int x  = node.at.x;
	const int y  = node.at.y;
	const int z  = node.at.z;
	const int sz = node.size;
	verts[0] = VolVectorInt( x,      y,      z );
	verts[1] = VolVectorInt( x + sz, y,      z );
	verts[2] = VolVectorInt( x + sz, y,      z + sz );
	verts[3] = VolVectorInt( x,      y,      z + sz );
	verts[4] = VolVectorInt( x,      y + sz, z );
	verts[5] = VolVectorInt( x + sz, y + sz, z );
	verts[6] = VolVectorInt( x + sz, y + sz, z + sz );
	verts[7] = VolVectorInt( x,      y + sz, z + sz );

	for ( int i=0; i<8; i++ )
	{
		// Value at unwarped position.
		const VolVectorInt & vert_int = verts[i];
		const Vector3d vert_d         = geometry->convert( verts[i], _scale );
		// Fill in node values.
		node.vertices_int[i] = verts[i];
		node.vertices[i]     = vert_d;
		// Initialize values with all zeros.
		node.values[i]       = 0.0;
	}

	static const int edges[][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0},
								   {4, 5}, {5, 6}, {6, 7}, {7, 4},
								   {0, 4}, {1, 5}, {2, 6}, {3, 7} };
	for ( int i=0; i<12; i++ )
	{
		node.values[i] = 0;
	}

	bool edge_intersects[12];
	for ( int i=0; i<12; i++ )
	{
		const int ind_a = edges[i][0];
		const int ind_b = edges[i][1];
		const Vector3d & a = node.vertices[ ind_a ];
		const Vector3d & b = node.vertices[ ind_b ];
		const bool intersects = _assign_edge_values( a, b, node.values[ind_a], node.values[ind_b], intersection_pts[i], source );
		edge_intersects[i] = intersects;
	}

	// There are edges which do not intersect.
	// If at least one edge has intersection,
	// after this operation all values should be assigned.
	for ( int i=0; i<12; i++ )
	{
		const bool intersects = edge_intersects[i];
		if ( intersects )
			continue;
		const int ind_a = edges[i][0];
		const int ind_b = edges[i][1];
		for ( int j=0; j<12; j++ )
		{
			if (i==j)
				continue;
			const bool sec_intersects = edge_intersects[j];
			if ( !sec_intersects )
				continue;
			const int sec_ind_a = edges[j][0];
			const int sec_ind_b = edges[j][1];
			if (ind_a == sec_ind_a)
				node.values[ind_a] += node.values[sec_ind_b];
			else if (ind_a == sec_ind_b)
				node.values[ind_a] += node.values[sec_ind_a];
			else if (ind_b == sec_ind_a)
				node.values[ind_b] += node.values[sec_ind_b];
			else if (ind_b == sec_ind_b)
				node.values[ind_b] += node.values[sec_ind_a];
		}
	}
}

bool VolMc::_assign_edge_values( const Vector3d & a, const Vector3d & b, Float & va, Float & vb, Vector3d & at, const VolSource * source ) const
{
	Vector3d norm;
	const bool ok = source->intersects( a, b, va, vb, at );
	return ok;
}

void VolMc::_create_faces( const VolMcNode & node, const Vector3d * intersection_pts )
{
	uint32_t cube_index = 0;
	for ( int i=0; i<8; i++ )
	{
		const Float value = node.values[i];
		if ( value >= 0.0 )
			cube_index |= (1 << i);
	}
	const int edge = VolMcTables::EDGES[cube_index];
	VolEdgeInt edges_int[12];
	if ( edge & 1 )
	{
		edges_int[0] = VolEdgeInt( node.vertices_int[0], node.vertices_int[1] );
	}
	if ( edge & 2 )
	{
		edges_int[1] = VolEdgeInt( node.vertices_int[1], node.vertices_int[2] );
	}
	if ( edge & 4 )
	{
		edges_int[2] = VolEdgeInt( node.vertices_int[2], node.vertices_int[3] );
	}
	if ( edge & 8 )
	{
		edges_int[3] = VolEdgeInt( node.vertices_int[3], node.vertices_int[0] );
	}
	if ( edge & 16 )
	{
		edges_int[4] = VolEdgeInt( node.vertices_int[4], node.vertices_int[5] );
	}
	if ( edge & 32 )
	{
		edges_int[5] = VolEdgeInt( node.vertices_int[5], node.vertices_int[6] );
	}
	if ( edge & 64 )
	{
		edges_int[6] = VolEdgeInt( node.vertices_int[6], node.vertices_int[7] );
	}
	if ( edge & 128 )
	{
		edges_int[7] = VolEdgeInt( node.vertices_int[7], node.vertices_int[4] );
	}
	if ( edge & 256 )
	{
		edges_int[8] = VolEdgeInt( node.vertices_int[0], node.vertices_int[4] );
	}
	if ( edge & 512 )
	{
		edges_int[9] = VolEdgeInt( node.vertices_int[1], node.vertices_int[5] );
	}
	if ( edge & 1024 )
	{
		edges_int[10] = VolEdgeInt( node.vertices_int[2], node.vertices_int[6] );
	}
	if ( edge & 2048 )
	{
		edges_int[11] = VolEdgeInt( node.vertices_int[3], node.vertices_int[7] );
	}

	const int * indices = VolMcTables::TRIANGLES[cube_index];
	for ( int i=0; indices[i] != -1; i+=3 )
	{
		const int ind_a = indices[i];
		const int ind_b = indices[i+1];
		const int ind_c = indices[i+2];

		const Vector3d & a = intersection_pts[ind_a];
		const Vector3d & b = intersection_pts[ind_b];
		const Vector3d & c = intersection_pts[ind_c];

		const VolEdgeInt & edge_a = edges_int[ind_a];
		const VolEdgeInt & edge_b = edges_int[ind_b];
		const VolEdgeInt & edge_c = edges_int[ind_c];

		//const Vector3 fa( a.x_, a.y_, a.z_ );
		//const Vector3 fb( b.x_, b.y_, b.z_ );
		//const Vector3 fc( c.x_, c.y_, c.z_ );
		//const Face3 f( fa, fb, fc );

		const VolFace face( a, b, c, edge_a, edge_b, edge_c );
		_faces.push_back( face );
	}
}







}

