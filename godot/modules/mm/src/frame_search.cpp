
#include "frame_search.h"
#include "core/sort_array.h"
#include "core/math/math_funcs.h"

namespace MM
{

static const Float EPS = 1.0e-9;

void FrameSearch::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_dims", "int"), &FrameSearch::set_dims, Variant::NIL );
	ClassDB::bind_method( D_METHOD("dims"),            &FrameSearch::dims,     Variant::INT );
	ClassDB::bind_method( D_METHOD("clear"),           &FrameSearch::clear,    Variant::INT );
	ClassDB::bind_method( D_METHOD("append",   "desc"),  &FrameSearch::append,   Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_desc", "int", "desc" ),  &FrameSearch::set_desc,   Variant::NIL );
	ClassDB::bind_method( D_METHOD("inv_std" ),  &FrameSearch::inv_std,    Variant::POOL_REAL_ARRAY );
	ClassDB::bind_method( D_METHOD("inv_ampl" ), &FrameSearch::inv_ampl,   Variant::POOL_REAL_ARRAY );
	ClassDB::bind_method( D_METHOD("build_tree" ), &FrameSearch::build_tree, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_weight", "int", "real_t" ), &FrameSearch::set_weight, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_weights", "vector real_t" ), &FrameSearch::set_weights, Variant::NIL );
	ClassDB::bind_method( D_METHOD("weights" ), &FrameSearch::weights, Variant::POOL_REAL_ARRAY );
	ClassDB::bind_method( D_METHOD("nearest", "vector real_t" ), &FrameSearch::nearest, Variant::REAL );
	ClassDB::bind_method( D_METHOD("nearest_dist" ), &FrameSearch::nearest_dist, Variant::REAL );
	ClassDB::bind_method( D_METHOD("nearest_ind" ),  &FrameSearch::nearest_ind,  Variant::INT );
	ClassDB::bind_method( D_METHOD("dist", "vector real", "int" ), &FrameSearch::dist,  Variant::REAL );
	ClassDB::bind_method( D_METHOD("dist2", "int", "int" ), &FrameSearch::dist2,  Variant::REAL );
}

FrameSearch::FrameSearch()
	: Reference()
{
	int   nearest_index_ = -1;
	Float nearest_dist_  = 0.0;

}

FrameSearch::~FrameSearch()
{
}

bool FrameSearch::set_dims( int dims )
{
	const bool res = tree_.set_dims( dims );
	return res;
}

int FrameSearch::dims() const
{
	const int res = tree_.dims();
	return res;
}

void FrameSearch::clear()
{
	tree_.clear();
}

void FrameSearch::append( const PoolVector<real_t> & desc )
{
	const int sz = desc.size();
	const int dims = tree_.dims();
	const int qty = (sz <= dims) ? sz : dims;
	FrameDesc d;
	for ( int i=0; i<qty; i++ )
		d.data[i] = desc.get( i );
	tree_.append( d );
}

void FrameSearch::set_desc( int index, const PoolVector<real_t> & desc )
{
	const int sz = desc.size();
	const int dims = tree_.dims();
	const int qty = (sz <= dims) ? sz : dims;
	FrameDesc d;
	for ( int i=0; i<qty; i++ )
		d.data[i] = desc.get( i );
	tree_.data_.ptrw()[index] = d;
}

PoolVector<real_t> FrameSearch::inv_std()
{
	FrameDesc mean, std;
	const int dims = tree_.dims();
	for ( size_t i=0; i<dims; i++ )
	{
		mean.data[i]    = 0.0;
		std.data[i] = 0.0;
	}
	const size_t qty = tree_.data_.size();
	for ( size_t i=0; i<qty; i++ )
	{
		for ( size_t j=0; j<dims; j++ )
		{
			const Float v = tree_.data_.ptr()[i].data[j];
			mean.data[j] += v;
		}
	}
	for ( size_t j=0; j<dims; j++ )
	{
		const Float m = mean.data[j] / static_cast<Float>( qty );
		mean.data[j] = m;
	}

	for ( size_t i=0; i<qty; i++ )
	{
		for ( size_t j=0; j<dims; j++ )
		{
			const Float v = tree_.data_.ptr()[i].data[j];
			const Float m = mean.data[j];
			const Float dv = v - m;
			std.data[j] += dv*dv;
		}
	}
	for ( size_t j=0; j<dims; j++ )
	{
		const Float s = Math::sqrt( std.data[j] / static_cast<Float>( qty ) );
		if ( s > EPS )
			std.data[j] = 1.0/s;
		else
			std.data[j] = 1.0;
	}

	PoolVector<real_t> res;
	res.resize( dims );
	for ( int i=0; i<dims; i++ )
		res.set( i, std.data[i] );

	return res;
}

PoolVector<real_t> FrameSearch::inv_ampl()
{
	FrameDesc mean, std;
	FrameDesc vmin, vmax;
	PoolVector<bool> assigned;
	const int dims = tree_.dims();
	assigned.resize( dims );
	for ( size_t i=0; i<dims; i++ )
	{
		mean.data[i] = 0.0;
		std.data[i]  = 0.0;
		assigned.set( i, false );
	}
	const size_t qty = tree_.data_.size();
	for ( size_t i=0; i<qty; i++ )
	{
		for ( size_t j=0; j<dims; j++ )
		{
			const Float v = tree_.data_.ptr()[i].data[j];
			mean.data[j] += v;

			const bool ass = assigned.get( j );
			if ( !ass )
			{
				vmin.data[j] = vmax.data[j] = v;
				assigned.set( j, true );
			}
			else
			{
				double & currMin = vmin.data[j];
				if ( v < currMin )
					currMin = v;
				double & currMax = vmax.data[j];
				if ( v > currMax )
					currMax = v;
			}
		}
	}
	for ( size_t j=0; j<dims; j++ )
	{
		const Float m = mean.data[j] / static_cast<Float>( qty );
		mean.data[j] = m;

		double & currMin = vmin.data[j];
		currMin = (currMin - m) + m;

		double & currMax = vmax.data[j];
		currMax = (currMax - m) + m;
	}

	for ( size_t j=0; j<dims; j++ )
	{
		const Float s = vmax.data[j] - vmin.data[j];
		if ( s > EPS )
			std.data[j] = 1.0/s;
		else
			std.data[j] = 1.0;
	}

	PoolVector<real_t> res;
	res.resize( dims );
	for ( int i=0; i<dims; i++ )
		res.set( i, std.data[i] );

	return res;
}

void FrameSearch::build_tree()
{
	tree_.build_tree();
}

void FrameSearch::set_weight( int index, real_t w )
{
	tree_.set_weight( index, w );
}

void FrameSearch::set_weights( const PoolVector<real_t> & w )
{
	const int sz = w.size();
	const int dims = tree_.dims();
	const int qty = (sz <= dims) ? sz : dims;
	FrameDesc d;
	for ( int i=0; i<qty; i++ )
		d.data[i] = w.get( i );
	tree_.set_weight( d );
}

PoolVector<real_t> FrameSearch::weights() const
{
	const int dims = tree_.dims();
	PoolVector<real_t> w;
	w.resize( dims );
	for ( int i=0; i<dims; i++ )
		w.set( i, tree_.weight_.data[i] );
	return w;
}

real_t FrameSearch::nearest( const PoolVector<real_t> & desc )
{
	const int sz = desc.size();
	const int dims = tree_.dims();
	const int qty = (sz <= dims) ? sz : dims;
	FrameDesc d;
	for ( int i=0; i<qty; i++ )
		d.data[i] = desc.get( i );

	nearest_index_ = tree_.nearest( d, nearest_dist_ );

	return nearest_dist_;
}

real_t FrameSearch::nearest_dist() const
{
	return nearest_dist_;
}

int FrameSearch::nearest_ind() const
{
	return nearest_index_;
}

real_t FrameSearch::dist( const PoolVector<real_t> & desc, int index )
{
	const int sz = desc.size();
	const int dims = tree_.dims();
	const int qty = (sz <= dims) ? sz : dims;
	FrameDesc d;
	for ( int i=0; i<qty; i++ )
		d.data[i] = desc.get( i );

	const Float ind_dist = tree_.dist( d, index );

	return ind_dist;
}

real_t FrameSearch::dist2( int index_a, int index_b )
{
	const Float ind_dist = tree_.dist( index_a, index_b );
	return ind_dist;
}


}






