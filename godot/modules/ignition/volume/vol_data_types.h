
#ifndef __VOL_DATA_TYPES_H_
#define __VOL_DATA_TYPES_H_

#include "vector3d.h"
#include "data_types.h"

namespace Ign
{


class VolMcNode;

class VolVectorInt
{
public:
	Integer x, y, z;
	VolVectorInt()
	{
		x = y = z = 0;
	}
	const VolVectorInt & operator=( const VolVectorInt & inst )
	{
		if ( this != &inst )
		{
			x = inst.x;
			y = inst.y;
			z = inst.z;
		}
		return *this;
	}
	bool operator==( const VolVectorInt & inst ) const
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
	bool operator!=( const VolVectorInt & inst ) const
	{
		const bool ret = !(*this == inst);
		return ret;
	}
	bool operator<( const VolVectorInt & inst ) const
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
	const VolVectorInt operator+( const VolVectorInt & b )
	{
		const VolVectorInt ret( x+b.x, y+b.y, z+b.z );
		return ret;
	}
	const VolVectorInt operator+( const Integer b )
	{
		const VolVectorInt ret( x+b, y+b, z+b );
		return ret;
	}
	VolVectorInt( const VolVectorInt & inst )
	{
		*this = inst;
	}
	VolVectorInt( Integer x, Integer y, Integer z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	const VolVectorInt operator*( const Integer & c )
	{
		const VolVectorInt ret( x*c, y*c, z*c );
		return ret;
	}
	const VolVectorInt operator*=( const Integer & c )
	{
		x *= c;
		y *= c;
		z *= c;
		return *this;
	}

};

VolVectorInt operator+( const VolVectorInt & a, const VolVectorInt & b );
VolVectorInt operator+( const VolVectorInt & a, Integer b );





class VolEdgeInt
{
public:
	VolVectorInt a, b;

	VolEdgeInt()
	{}
	VolEdgeInt( const VolVectorInt & aa, const VolVectorInt & bb )
	{
		a = aa;
		b = bb;
	}
	~VolEdgeInt()
	{}
	const VolEdgeInt & operator=( const VolEdgeInt & inst )
	{
		if ( this != &inst )
		{
			a = inst.a;
			b = inst.b;
		}
		return *this;
	}
	VolEdgeInt( const VolEdgeInt & inst )
	{
		*this = inst;
	}

	bool operator<( const VolEdgeInt & other ) const
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


class VolFace
{
public:
	//Face3       face;
	Vector3d    vertices[3];
	Vector3d    normals[3];
	Vector3d    tangent;
	// Node edges on which face vertices lay.
	VolEdgeInt node_edges[3];
	// Cell this face has been created with.
	VolMcNode * node;

	VolFace()
	{
		node = nullptr;
	}
	VolFace( const Vector3d & va, const Vector3d & vb, const Vector3d & vc, const VolEdgeInt & na, const VolEdgeInt & nb, const VolEdgeInt & nc )
	{
		//face = f;
		vertices[0]   = va;
		vertices[1]   = vb;
		vertices[2]   = vc;

		node_edges[0] = na;
		node_edges[1] = nb;
		node_edges[2] = nc;

		node = nullptr;
	}
	~VolFace()
	{}
	const VolFace & operator=( const VolFace & inst )
	{
		if ( this != &inst )
		{
			//face = inst.face;
			for ( int i=0; i<3; i++ )
			{
				vertices[i]   = inst.vertices[i];
				normals[i]    = inst.normals[i];
				node_edges[i] = inst.node_edges[i];
			}
			tangent      = inst.tangent;
			node         = inst.node;
		}

		return *this;
	}
	VolFace( const VolFace & inst )
	{
		*this = inst;
	}
	Vector3d normal() const
	{
		const Vector3d a = vertices[0] - vertices[2];
		const Vector3d b = vertices[0] - vertices[1];
		const Vector3d c = a.CrossProduct( b );
		const Vector3d n = c.Normalized();
		return n;
	}
	bool intersects_ray(const Vector3d & p_from, const Vector3d &p_dir, Vector3d *p_intersection = nullptr) const;
	bool intersects_segment(const Vector3d & p_from, const Vector3d &p_to, Vector3d *p_intersection = nullptr) const;
};

bool operator<( const VolFace & a, const VolFace & b );




class NormalAndQty
{
public:
	Vector3d norm;
	Integer  qty;

	NormalAndQty()
	{
		norm = Vector3d::ZERO;
		qty  = 0;
	}
	NormalAndQty( const Vector3d & n )
	{
		norm = n;
		qty  = 1;
	}
	~NormalAndQty()
	{}
	const NormalAndQty & operator=( const NormalAndQty & inst )
	{
		if ( this != &inst )
		{
			norm = inst.norm;
			qty  = inst.qty;
		}
		return *this;
	}
	NormalAndQty( const NormalAndQty & inst )
	{
		*this = inst;
	}
};


}


#endif

