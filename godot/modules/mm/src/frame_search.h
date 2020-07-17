
#ifndef __FRAME_SEARCH_H_
#define __FRAME_SEARCH_H_

#include "core/vector.h"

namespace MM
{

#define MAX_SIZE  32
typedef double Float;

struct FrameDesc
{
	Float data[MAX_SIZE];
};

struct TreeNode
{
	// In FrameDesc array.
	int frame_ind_;
	// In nodes array.
	int left_ind_;
	int right_ind_;
};

class KdTree
{
public:
	KdTree();
	~KdTree();

	void setDims( int sz );
	void clear();
	void append( const FrameDesc & data );
	void setWeight( const FrameDesc & data );
	void setWeight( int ind, Float v );

	Float dist( const FrameDesc & a, const FrameDesc & b );

	int               sz_;
	FrameDesc         weight_;
	Vector<FrameDesc> data_;
	Vector<TreeNode>  nodes_;
};

}



#endif



