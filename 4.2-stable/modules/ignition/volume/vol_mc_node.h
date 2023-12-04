
#ifndef __VOL_MC_NODE_H_
#define __VOL_MC_NODE_H_

#include "data_types.h"
#include "vol_data_types.h"


namespace Ign
{

class VolMcNode
{
public:
	VolVectorInt at;
	Integer   size;
	VolVectorInt vertices_int[8];
	Vector3d  vertices[8];
	Float     values[8];


	VolMcNode()
	{
		at   = VolVectorInt();
		size = 1;
	}

	~VolMcNode()
	{}

	const VolMcNode & operator=( const VolMcNode & inst )
	{
		if ( this != &inst )
		{
			at   = inst.at;
			size = inst.size;
			for ( int i=0; i<8; i++ )
			{
				vertices_int[i]      = inst.vertices_int[i];
				vertices[i]          = inst.vertices[i];
				values[i]            = inst.values[i];
			}
		}
		return * this;
	}

	bool operator==( const VolMcNode & inst ) const
	{
		if (this == &inst)
			return true;
		if (at != inst.at)
			return false;
		if (size != inst.size)
			return false;
		return true;
	}

	bool operator<( const VolMcNode & inst ) const
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

	VolMcNode( const VolMcNode & inst )
	{
		*this = inst;
	}

	VolVectorInt top() const
	{
		const VolVectorInt ret = at + size;
		return ret;
	}

	bool contains( const VolMcNode & node ) const
	{
		if (node.at.x < at.x)
			return false;
		if (node.at.y < at.y)
			return false;
		if (node.at.y < at.y)
			return false;

		const VolVectorInt node_top = node.top();
		const VolVectorInt own_top  = top();

		if ( node_top.x > own_top.x )
			return false;
		if ( node_top.y > own_top.y )
			return false;
		if ( node_top.z > own_top.z )
			return false;

		return true;
	}

};

}





#endif



