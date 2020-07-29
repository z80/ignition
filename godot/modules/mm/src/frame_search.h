
#ifndef __FRAME_SEARCH_H_
#define __FRAME_SEARCH_H_

#include "core/reference.h"
#include "kd_tree.h"

namespace MM
{

class FrameSearch: public Reference
{
	GDCLASS(FrameSearch, Reference);
	OBJ_CATEGORY("MM");
protected:
	static void _bind_methods();

public:
	FrameSearch();
	~FrameSearch();

	bool set_dims( int dims );
	int dims() const;

	void clear();
	void append( const PoolVector<real_t> & desc );
	// Crazification method to force it to not be selected.
	void set_desc( int index, const PoolVector<real_t> & desc );
	// Normalization methods.
	PoolVector<real_t> inv_std();
	PoolVector<real_t> inv_ampl();

	void build_tree();

	void set_weight( int index, real_t w );
	void set_weights( const PoolVector<real_t> & w );
	PoolVector<real_t> weights() const;

	real_t nearest( const PoolVector<real_t> & desc );
	real_t nearest_dist() const;
	int    nearest_ind() const;

	real_t dist( const PoolVector<real_t> & desc, int index );
	real_t dist2( int index_a, int index_b );

private:
	KdTree tree_;
	int   nearest_index_;
	Float nearest_dist_;
};


}



#endif



