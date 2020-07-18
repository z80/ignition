
#ifndef __FRAME_SEARCH_H_
#define __FRAME_SEARCH_H_

#include "core/vector.h"

namespace MM
{

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

	void set_dims( int sz );
	int  dims() const;
	void clear();
	void append( const FrameDesc & data );
	void set_weight( const FrameDesc & data );
	void set_weight( int ind, Float v );

	Float dist( const FrameDesc & a, const FrameDesc & b );

	void build_tree();

	int nearest( const FrameDesc & point, Float & dist );
	Float dist( const FrameDesc & point, int index );

	int               dims_;
	FrameDesc         weight_;
	Vector<FrameDesc> data_;
	Vector<TreeNode>  nodes_;

	int root_;
	int best_ind_;
	Float best_dist_;
	int visited_qty_;


private:
	int make_tree( int begin, int end, int index );
	void nearest_internal( const FrameDesc & point, int root_ind, int dim_index, int & best_ind, Float & best_dist );

};

}



#endif



