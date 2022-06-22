
#ifndef __CUBE_TYPES_H_
#define __CUBE_TYPES_H_

#include "data_types.h"
#include "vector3d.h"
#include "core/math/face3.h"


namespace Ign
{

class VectorInt
{
public:
	int x, y, z;
	VectorInt()
	{
		x = y = z = 0;
	}
	const VectorInt & operator=( const VectorInt & inst )
	{
		if ( this != &inst )
		{
			x = inst.x;
			y = inst.y;
			z = inst.z;
		}
		return *this;
	}
	bool operator==( const VectorInt & inst ) const
	{
		if ( this == &inst )
			return true;
		if (x != inst.x)
			return false;
		if (y != inst.y)
			return false;
		if (z != inst.z)
			return false;
		return true;
	}
	bool operator!=( const VectorInt & inst ) const
	{
		const bool ret = !(*this == inst);
		return ret;
	}
	bool operator<( const VectorInt & inst ) const
	{
		if ( x < inst.x )
			return true;
		else if ( x > inst.x )
			return false;

		if ( y < inst.y )
			return true;
		else if ( y > inst.y )
			return false;

		if ( z < inst.z )
			return true;
		else if ( z > inst.z )
			return false;

		return false;
	}
	const VectorInt operator+( const VectorInt & b )
	{
		const VectorInt ret( x+b.x, y+b.y, z+b.z );
		return ret;
	}
	VectorInt( const VectorInt & inst )
	{
		*this = inst;
	}
	VectorInt( int x, int y, int z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

};


class MarchingNode
{
public:
	VectorInt at;
	int       size;
	VectorInt vertices_int[8];
	Vector3d  vertices[8];
	Vector3d  vertices_unscaled[8];
	Float     values[8];


	MarchingNode()
	{
		at   = VectorInt();
		size = 1;
		for ( int i=0; i<8; i++ )
			values[i] = 0.0;
	}

	~MarchingNode()
	{}

	const MarchingNode & operator=( const MarchingNode & inst )
	{
		if ( this != &inst )
		{
			at   = inst.at;
			size = inst.size;
			for ( int i=0; i<8; i++ )
			{
				vertices_int[i]      = inst.vertices_int[i];
				vertices[i]          = inst.vertices[i];
				vertices_unscaled[i] = inst.vertices_unscaled[i];
				values[i]            = inst.values[i];
			}
		}
		return * this;
	}

	bool operator==( const MarchingNode & inst ) const
	{
		if (this == &inst)
			return true;
		if (at != inst.at)
			return false;
		if (size != inst.size)
			return false;
		return true;
	}

	bool operator<( const MarchingNode & inst ) const
	{
		if ( at.x < inst.at.x )
			return true;
		else if ( at.x > inst.at.x )
			return false;

		if ( at.y < inst.at.y )
			return true;
		else if ( at.y > inst.at.y )
			return false;

		if ( at.z < inst.at.z )
			return true;
		else if ( at.z > inst.at.z )
			return false;

		if ( size < inst.size )
			return true;
		else if ( size > inst.size )
			return false;

		return false;
	}

	MarchingNode( const MarchingNode & inst )
	{
		*this = inst;
	}

	bool has_surface( const Float iso_level ) const
	{
		int above_qty = 0;
		int below_qty = 0;
		for ( int i=0; i<8; i++ )
		{
			const Float v = values[i];
			if (v > iso_level)
				above_qty += 1;
			else if (v < iso_level)
				below_qty += 1;

			if ( (above_qty > 0) && (below_qty > 0) )
				return true;
		}

		return false;
	}
};


class NodeEdgeInt
{
public:
	VectorInt a, b;

	NodeEdgeInt()
	{}
	NodeEdgeInt( const VectorInt & aa, const VectorInt & bb )
	{
		a = aa;
		b = bb;
	}
	~NodeEdgeInt()
	{}
	const NodeEdgeInt & operator=( const NodeEdgeInt & inst )
	{
		if ( this != &inst )
		{
			a = inst.a;
			b = inst.b;
		}
		return *this;
	}
	NodeEdgeInt( const NodeEdgeInt & inst )
	{
		*this = inst;
	}

	bool operator<( const NodeEdgeInt & other ) const
	{
		if ( a.x < other.a.x )
			return true;
		else if ( a.x > other.a.x )
			return false;

		if ( a.y < other.a.y )
			return true;
		else if ( a.y > other.a.y )
			return false;

		if ( a.z < other.a.z )
			return true;
		else if ( a.z > other.a.z )
			return false;


		if ( b.x < other.b.x )
			return true;
		else if ( b.x > other.b.x )
			return false;

		if ( b.y < other.b.y )
			return true;
		else if ( b.y > other.b.y )
			return false;

		if ( b.z < other.b.z )
			return true;
		else if ( b.z > other.b.z )
			return false;

		return false;
	}
};



class NodeFace
{
public:
	Face3       face;
	// Node edges on which face vertices lay.
	NodeEdgeInt node_edges[3];

	NodeFace()
	{}
	NodeFace( const Face3 & f, const NodeEdgeInt & na, const NodeEdgeInt & nb, const NodeEdgeInt & nc )
	{
		face = f;
		node_edges[0] = na;
		node_edges[1] = nb;
		node_edges[2] = nc;
	}
	~NodeFace()
	{}
	const NodeFace & operator=( const NodeFace & inst )
	{
		if ( this != &inst )
		{
			face = inst.face;
			for ( int i=0; i<3; i++ )
			{
				node_edges[i] = inst.node_edges[i];
			}
		}

		return *this;
	}
	NodeFace( const NodeFace & inst )
	{
		*this = inst;
	}
};




class NormalsAndQty
{
public:
	Vector3d norms;
	int      qty;

	NormalsAndQty()
	{
		norms = Vector3d::ZERO;
		qty   = 0;
	}
	NormalsAndQty( const Vector3d & n )
	{
		norms = n;
		qty   = 1;
	}
	~NormalsAndQty()
	{}
	const NormalsAndQty & operator=( const NormalsAndQty & inst )
	{
		if ( this != &inst )
		{
			norms = inst.norms;
			qty   = inst.qty;
		}
		return *this;
	}
	NormalsAndQty( const NormalsAndQty & inst )
	{
		*this = inst;
	}
};




}





#endif












