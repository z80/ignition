
#include "vol_mc.h"
#include "vol_mc_node.h"
#include "vol_geometry.h"
#include "vol_source.h"

#include "vol_mc_tables.h"

namespace Ign
{

VolMc::VolMc()
	: _size( 0.1 ),
	  _resolution( 1 )
{
}

VolMc::~VolMc()
{
}

bool VolMc::build_surface( const VolGeometry * geometry, const VolSource * source, Float size, Integer resolution )
{
	_size       = size;
	_resolution = resolution;

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
	_scale = _size / static_cast<Float>(_resolution);
}

void VolMc::_traverse_nodes_brute_force( const VolGeometry * geometry, const VolSource * source )
{
	Vector3d intersection_pts[12];
	bool     pts_assigned[12];
	for ( int ix=0; ix<_resolution; ix+=1 )
	{
		for ( int iy=0; iy<_resolution; iy+=1 )
		{
			for ( int iz=0; iz<_resolution; iz+=1 )
			{
				VolMcNode node;
				node.at   = VolVectorInt( ix, iy, iz );
				node.size = 1;
				for ( int i=0; i<12; i++ )
					pts_assigned[i] = false;
				const bool ok = _compute_node_values( node, intersection_pts, geometry, source, pts_assigned );
				if ( ok )
					_create_faces( node, intersection_pts, pts_assigned );
			}
		}
	}
}

bool VolMc::_compute_node_values( VolMcNode & node, Vector3d * intersection_pts, const VolGeometry * geometry, const VolSource * source, bool * pts_assigned )
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

	bool edge_intersects[12];
	int intersections_qty = 0;
	for ( int i=0; i<12; i++ )
	{
		const int ind_a = edges[i][0];
		const int ind_b = edges[i][1];
		const Vector3d & a = node.vertices[ ind_a ];
		const Vector3d & b = node.vertices[ ind_b ];
		const bool intersects = _assign_edge_values( a, b, node.values[ind_a], node.values[ind_b], intersection_pts[i], source );
		edge_intersects[i] = intersects;
		pts_assigned[i] = intersects;
		if (intersects)
			intersections_qty += 1;
	}
	if (intersections_qty < 3)
		return false;

	static const Float EPS = 0.000001;

	while ( true )
	{
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

			const Float val_a = node.values[ind_a];
			const Float val_b = node.values[ind_b];
			if ( Math::abs(val_a) < EPS )
				node.values[ind_a] += val_b;
			if ( Math::abs(val_b) < EPS )
				node.values[ind_b] += val_a;
		}

		int zero_qty = 0;
		int intersections_qty = 0;
		for ( int i=0; i<12; i++ )
		{
			const bool intersects = edge_intersects[i];
			const int ind_a = edges[i][0];
			const int ind_b = edges[i][1];
			const Float val_a = node.values[ind_a];
			const Float val_b = node.values[ind_b];

			if ( ( Math::abs(val_a) < EPS ) && ( Math::abs(val_b) < EPS ) )
				zero_qty += 1;
			else if ( (val_a * val_b) < 0.0 )
				intersections_qty += 1;
		}

		if ( intersections_qty == 0 )
			break;
		else if ( zero_qty == 0 )
			break;
	}

	return true;
}

bool VolMc::_assign_edge_values( const Vector3d & a, const Vector3d & b, Float & va, Float & vb, Vector3d & at, const VolSource * source ) const
{
	Vector3d norm;
	const bool ok = source->intersects( a, b, va, vb, at );
	return ok;
}

void VolMc::_create_faces( const VolMcNode & node, const Vector3d * intersection_pts, const bool * pts_assigned )
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
		if (!pts_assigned[0])
			int i=0;
	}
	if ( edge & 2 )
	{
		edges_int[1] = VolEdgeInt( node.vertices_int[1], node.vertices_int[2] );
		if (!pts_assigned[1])
			int i=0;
	}
	if ( edge & 4 )
	{
		edges_int[2] = VolEdgeInt( node.vertices_int[2], node.vertices_int[3] );
		if (!pts_assigned[2])
			int i=0;
	}
	if ( edge & 8 )
	{
		edges_int[3] = VolEdgeInt( node.vertices_int[3], node.vertices_int[0] );
		if (!pts_assigned[3])
			int i=0;
	}
	if ( edge & 16 )
	{
		edges_int[4] = VolEdgeInt( node.vertices_int[4], node.vertices_int[5] );
		if (!pts_assigned[4])
			int i=0;
	}
	if ( edge & 32 )
	{
		edges_int[5] = VolEdgeInt( node.vertices_int[5], node.vertices_int[6] );
		if (!pts_assigned[5])
			int i=0;
	}
	if ( edge & 64 )
	{
		edges_int[6] = VolEdgeInt( node.vertices_int[6], node.vertices_int[7] );
		if (!pts_assigned[6])
			int i=0;
	}
	if ( edge & 128 )
	{
		edges_int[7] = VolEdgeInt( node.vertices_int[7], node.vertices_int[4] );
		if (!pts_assigned[7])
			int i=0;
	}
	if ( edge & 256 )
	{
		edges_int[8] = VolEdgeInt( node.vertices_int[0], node.vertices_int[4] );
		if (!pts_assigned[8])
			int i=0;
	}
	if ( edge & 512 )
	{
		edges_int[9] = VolEdgeInt( node.vertices_int[1], node.vertices_int[5] );
		if (!pts_assigned[9])
			int i=0;
	}
	if ( edge & 1024 )
	{
		edges_int[10] = VolEdgeInt( node.vertices_int[2], node.vertices_int[6] );
		if (!pts_assigned[10])
			int i=0;
	}
	if ( edge & 2048 )
	{
		edges_int[11] = VolEdgeInt( node.vertices_int[3], node.vertices_int[7] );
		if (!pts_assigned[11])
			int i=0;
	}

	const int * indices = VolMcTables::TRIANGLES[cube_index];
	for ( int i=0; indices[i] != -1; i+=3 )
	{
		const int ind_a = indices[i];
		const int ind_b = indices[i+1];
		const int ind_c = indices[i+2];

		const bool assigned_a = pts_assigned[ind_a];
		if ( !assigned_a )
			continue;
		const bool assigned_b = pts_assigned[ind_b];
		if ( !assigned_b )
			continue;
		const bool assigned_c = pts_assigned[ind_c];
		if ( !assigned_c )
			continue;

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

