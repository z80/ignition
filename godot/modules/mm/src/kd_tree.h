
#ifndef __KD_TREE_H_
#define __KD_TREE_H_

#include "core/vector.h"

namespace MM
{

typedef double Float;

static const int MAX_SIZE = 32;

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

	bool set_dims( int sz );
	int  dims() const;
	void clear();
	void append( const FrameDesc & data );
	void set_weight( const FrameDesc & data );
	void set_weight( int ind, Float v );

	Float dist( const FrameDesc & a, const FrameDesc & b ) const;

	void build_tree();

	int nearest( const FrameDesc & point, Float & dist );
	Float dist( const FrameDesc & point, int index ) const;
	Float dist( int index_a, int index_b ) const;

	int               dims_;
	FrameDesc         weight_;
	Vector<FrameDesc> data_;
	Vector<TreeNode>  nodes_;

	int root_;
	//int best_ind_;
	//Float best_dist_;
	int visited_qty_;


private:
	int make_tree_internal( int begin, int end, int index );
	void nearest_internal( const FrameDesc & point, int root_ind, int dim_index, int & best_ind, Float & best_dist );

};



}


#endif



