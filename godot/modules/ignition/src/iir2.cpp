
#include "iir2.h"

namespace Iir2
{

Iir2::Iir2()
{
	a[0] = 0.9800525082063365;
	a[1] = -1.9798515425143592;
	b[0] = 1.0;
	b[1] = 2.0;
	inv_gain = 2.0/19903.894842175476;
	for ( int i=0; i<6; i++ )
		xyv[i] = 0.0;
}

Iir2::~Iir2()
{
}

Iir2::Iir2( const Iir2 & inst )
{
	*this = inst;
}

const Iir2 & Iir2::operator=( const Iir2 & inst )
{
	if ( this != &inst )
	{
		a[0] = inst.a[0];
		a[1] = inst.a[1];
		b[0] = inst.b[0];
		b[1] = inst.b[1];
		inv_gain = inst.inv_gain;
		for ( int i=0; i<6; i++ )
			xyv[i] = inst.xyv[i];
	}
	return *this;
}

Float Iir2::operator()( Float v )
{
	int i, b, xp=0, yp=3, bqp=0;
	Float out = v * inv_gain;
	for (i=5; i>0; i--)
		xyv[i]=xyv[i-1];

	for (b=0; b<1; b++)
	{
		int len=2;
		xyv[xp] = out;
		for(i=0; i<len; i++)
		{
			out += xyv[xp+len-i] * this->b[bqp+i] - xyv[yp+len-i] * this->a[bqp+i];
		}

		bqp    += len;
		xyv[yp] = out;
		xp      = yp;
		yp     += len + 1;
	}
	return out;
}

void Iir2::reset_to( Float v, int iters_qty )
{
	for ( int i=0; i<iters_qty; i++ )
	{
		operator()( v );
	}
}

void Iir2::set_a0( Float a0 )
{
	a[0] = a0;
}

Float Iir2::get_a0() const
{
	return a[0];
}

void Iir2::set_a1( Float a1 )
{
	a[1] = a1;
}

Float Iir2::get_a1() const
{
	return a[1];
}

void Iir2::set_b0( Float b0 )
{
	b[0] = b0;
}

Float Iir2::get_b0() const
{
	return b[0];
}

void Iir2::set_b1( Float b1 )
{
	b[1] = b1;
}

Float Iir2::get_b1() const
{
	return b[1];
}

void Iir2::set_gain( Float gain )
{
	if ( gain > 0.00001 )
		inv_gain = 1.0 / gain;
	else
		inv_gain = 1.0;
}

Float Iir2::get_gain() const
{
	return (1.0/inv_gain);
}








Iir2s::Iir2s()
{
	a[0] = 0.9800525082063365;
	a[1] = -1.9798515425143592;
	b[0] = 1.0;
	b[1] = 2.0;
	gain = 0.5*19903.894842175476;

	reset_iters_qty = 10000;
}

Iir2s::~Iir2s()
{
}

Iir2s::Iir2s( const Iir2s & inst )
{
	*this = inst;
}

const Iir2s & Iir2s::operator=( const Iir2s & inst )
{
	if ( this != &inst )
	{
		a[0] = inst.a[0];
		a[1] = inst.a[1];
		b[0] = inst.b[0];
		b[1] = inst.b[1];
		gain = inst.gain;

		reset_iters_qty = inst.reset_iters_qty;

		const int qty = inst.filters.size();
		resize( qty );
		for ( int i=0; i<qty; i++ )
		{
			const Iir2 & from = inst.filters.ptr()[i];
			Iir2 & to         = filters.ptrw()[i];
			to = from;
		}
	}

	return *this;
}

void Iir2s::resize( int sz )
{
	filters.resize( sz );
	update_coeffs();
}

int Iir2s::size() const
{
	const int sz = filters.size();
	return sz;
}

const Array & Iir2s::operator()( const Array & v )
{
	const int qty_requested = v.size();
	const int qty_available = filters.size();
	if ( qty_available < qty_requested )
	{
		resize( qty_requested );
		for ( int i=qty_available; i<qty_requested; i++ )
		{
			Iir2 & filter = filters.ptrw()[i];
			const real_t x = v.get( i );
			filter.reset_to( x, reset_iters_qty );
		}
	}

	const int ret_sz = ret.size();
	if ( ret_sz != qty_requested )
		ret.resize( qty_requested );

	for ( int i=0; i<qty_requested; i++ )
	{
		const real_t x = v.get( i );
		Iir2 & filter = filters.ptrw()[i];
		const real_t y = filter( x );
		ret.set( i, y );
	}

	return ret;
}

void Iir2s::reset_to( const Array & v )
{
	operator()( v );
	const int qty = v.size();
	for ( int i=0; i<qty; i++ )
	{
		Iir2 & filter = filters.ptrw()[i];
		const real_t x = v.get( i );
		filter.reset_to( x, reset_iters_qty );
	}
}

void Iir2s::set_a0( Float a0 )
{
	a[0] = a0;
	update_coeffs();
}

Float Iir2s::get_a0() const
{
	return a[0];
}

void Iir2s::set_a1( Float a1 )
{
	a[1] = a1;
	update_coeffs();
}

Float Iir2s::get_a1() const
{
	return a[1];
}

void Iir2s::set_b0( Float b0 )
{
	b[0] = b0;
	update_coeffs();
}

Float Iir2s::get_b0() const
{
	return b[0];
}

void Iir2s::set_b1( Float b1 )
{
	b[1] = b1;
	update_coeffs();
}

Float Iir2s::get_b1() const
{
	return b[1];
}

void Iir2s::set_gain( Float gain )
{
	this->gain = gain;
	update_coeffs();
}

Float Iir2s::get_gain() const
{
	return gain;
}

void Iir2s::update_coeffs()
{
	const int qty = filters.size();
	for ( int i=0; i<qty; i++ )
	{
		Iir2 & filter = filters.ptrw()[i];
		filter.set_a0( a[0] );
		filter.set_a1( a[1] );
		filter.set_b0( b[0] );
		filter.set_b1( b[1] );
		filter.set_gain( gain );
	}
}














void Iir2sRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("resize", "size"), &Iir2sRef::resize );
	ClassDB::bind_method( D_METHOD("size" ),          &Iir2sRef::size );

	ClassDB::bind_method( D_METHOD("apply", "v" ),    &Iir2sRef::apply );
	ClassDB::bind_method( D_METHOD("reset_to", "v" ), &Iir2sRef::reset_to );

	ClassDB::bind_method( D_METHOD("set_a0", "a0" ), &Iir2sRef::set_a0 );
	ClassDB::bind_method( D_METHOD("get_a0" ),       &Iir2sRef::get_a0 );

	ClassDB::bind_method( D_METHOD("set_a1", "a1" ), &Iir2sRef::set_a1 );
	ClassDB::bind_method( D_METHOD("get_a1" ),       &Iir2sRef::get_a1 );

	ClassDB::bind_method( D_METHOD("set_b0", "b0" ), &Iir2sRef::set_b0 );
	ClassDB::bind_method( D_METHOD("get_b0" ),       &Iir2sRef::get_b0 );

	ClassDB::bind_method( D_METHOD("set_b1", "b1" ), &Iir2sRef::set_b1 );
	ClassDB::bind_method( D_METHOD("get_b1" ),       &Iir2sRef::get_b1 );

	ClassDB::bind_method( D_METHOD("set_gain", "gain" ), &Iir2sRef::set_gain );
	ClassDB::bind_method( D_METHOD("get_gain" ),         &Iir2sRef::get_gain );

	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "a0" ),   "set_a0",   "get_a0" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "a1" ),   "set_a1",   "get_a1" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "b0" ),   "set_b0",   "get_b0" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "b1" ),   "set_b1",   "get_b1" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "gain" ), "set_gain", "get_gain" );
}

Iir2sRef::Iir2sRef()
{
}

Iir2sRef::~Iir2sRef()
{
}

void Iir2sRef::resize( int sz )
{
	filters.resize( sz );
}

int Iir2sRef::size() const
{
	const int ret = filters.size();
	return ret;
}

const Array & Iir2sRef::apply( const Array & v )
{
	const Array & ret = filters( v );
	return ret;
}

void Iir2sRef::reset_to( const Array & v )
{
	filters.reset_to( v );
}

void Iir2sRef::set_a0( real_t a0 )
{
	filters.set_a0( a0 );
}

real_t Iir2sRef::get_a0() const
{
	const real_t ret = filters.get_a0();
	return ret;
}

void Iir2sRef::set_a1( real_t a1 )
{
	filters.set_a1( a1 );
}

real_t Iir2sRef::get_a1() const
{
	const real_t ret = filters.get_a1();
	return ret;
}

void Iir2sRef::set_b0( real_t b0 )
{
	filters.set_b0( b0 );
}

real_t Iir2sRef::get_b0() const
{
	const real_t ret = filters.get_b0();
	return ret;
}

void Iir2sRef::set_b1( real_t b1 )
{
	filters.set_b1( b1 );
}

real_t Iir2sRef::get_b1() const
{
	const real_t ret = filters.get_b1();
	return ret;
}

void Iir2sRef::set_gain( real_t gain )
{
	filters.set_gain( gain );
}

real_t Iir2sRef::get_gain() const
{
	const real_t ret = filters.get_gain();
	return ret;
}



}


