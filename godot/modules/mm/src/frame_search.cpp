
#include "frame_search.h"
#include "core/sort_array.h"

namespace MM
{

static const int MAX_SIZE = 32;








KdTree::KdTree()
{
	dims_ = MAX_SIZE;
}

KdTree::~KdTree()
{
}

void KdTree::set_dims( int sz )
{
	dims_ = sz;
}

int  KdTree::dims() const
{
	return dims_;
}

void KdTree::clear()
{
	data_.clear();
	nodes_.clear();
}

void KdTree::append( const FrameDesc & data )
{
	data_.push_back( data );
}

void KdTree::set_weight( const FrameDesc & data )
{
	weight_ = data;
}

void KdTree::set_weight( int ind, Float v )
{
	weight_.data[ind] = v;
}

Float KdTree::dist( const FrameDesc & a, const FrameDesc & b )
{
	Float d = 0.0;
	const int dims = dims_;
	for ( int i=0; i<dims; i++ )
	{
		const Float dv  = a.data[i] - b.data[i];
		const Float dv2 = dv*dv*weight_.data[i];
		d += dv2;
	}

	return d;
}

void KdTree::build_tree()
{
	root_       = -1;
	const int qty = data_.size();
	//Create and initialize nodes.
	nodes_.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		TreeNode & node = nodes_.ptrw()[i];
		node.frame_ind_ =  i;
		node.left_ind_  = -1;
		node.right_ind_ = -1;
	}
	root_ = make_tree( 0, qty, 0 );
}

int KdTree::nearest( const FrameDesc & point, Float & dist )
{
	int   best_ind = -1;
	Float best_dist = 0.0;
	nearest_internal( point, root_, 0, best_ind, best_dist );

	dist = best_dist;
	const TreeNode & node = nodes_.ptr()[best_ind];
	const int frame_best_ind = node.frame_ind_;
	return frame_best_ind;
}

Float KdTree::dist( const FrameDesc & point, int index )
{
	const FrameDesc & frame = data_.ptr()[index];
	const Float d = dist( point, frame );
	return d;
}

struct NthComparator
{
	int index;
	KdTree * tree;
	bool operator()( const TreeNode & a, const TreeNode & b ) const
	{
		const int ind_a = a.frame_ind_;
		const int ind_b = b.frame_ind_;
		const FrameDesc & fa = tree->data_.ptr()[ind_a];
		const FrameDesc & fb = tree->data_.ptr()[ind_b];
		const Float va = fa.data[index];
		const Float vb = fb.data[index];
		return (va < vb);
	}
};

int KdTree::make_tree( int begin, int end, int dim_index )
{
	if ( end <= begin )
		return -1;
	int n = begin + (end - begin)/2;
	SortArray<TreeNode, NthComparator> comparator;
	comparator.compare.index = dim_index;
	comparator.compare.tree  = this;
	comparator.nth_element( begin, end, n, nodes_.ptrw() );
	//std::nth_element(&nodes_[begin], &nodes_[n], &nodes_[end], node_cmp(index));
	const int new_index = (dim_index + 1) % dims_;
	TreeNode & node = nodes_.ptrw()[n];
	node.left_ind_  = make_tree( begin, n, new_index );
	node.right_ind_ = make_tree( n + 1, end, new_index );
	return n;
}

void KdTree::nearest_internal( const FrameDesc & point, int root_ind, int dim_index, int & best_ind, Float & best_dist )
{
	if ( root_ind < 0)
		return;
	++visited_qty_;

	const TreeNode & root_node = nodes_.ptr()[root_ind];
	const FrameDesc & root = data_.ptr()[root_node.frame_ind_];
	Float d = dist( root, point );
	if ( (best_ind < 0) || (d < best_dist) )
	{
		best_dist = d;
		best_ind  = root_ind;
	}
	if ( best_dist == 0.0 )
		return;

	const Float dx = root.data[dim_index] - point.data[dim_index];
	const Float dx2 = dx*dx*weight_.data[dim_index];
	const bool do_left_first = (dx > 0.0);
	const int next_dim_index = (dim_index + 1) % dims_;

	if ( do_left_first )
		nearest_internal( point, root_node.left_ind_, next_dim_index, best_ind, best_dist );
	else
		nearest_internal( point, root_node.right_ind_, next_dim_index, best_ind, best_dist );
	if ( dx2 >= best_dist_ )
		return;
	if ( !do_left_first )
		nearest_internal( point, root_node.right_ind_, next_dim_index, best_ind, best_dist );
	else
		nearest_internal( point, root_node.left_ind_, next_dim_index, best_ind, best_dist );
}






}



