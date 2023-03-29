
#ifndef __IIR2_H_
#define __IIR2_H_

#include "core/reference.h"


namespace Iir2
{

typedef double Float;

class Iir2
{
public:
	Iir2();
	~Iir2();

	Iir2( const Iir2 & inst );
	const Iir2 & operator=( const Iir2 & inst );

	Float operator()( Float v );
	void reset_to( Float v, int iters_qty );

	void set_a0( Float a0 );
	Float get_a0() const;

	void set_a1( Float a1 );
	Float get_a1() const;

	void set_b0( Float b0 );
	Float get_b0() const;

	void set_b1( Float b1 );
	Float get_b1() const;

	void set_gain( Float gain );
	Float get_gain() const;

public:
	Float a[2];
	Float b[1];
	Float inv_gain;
	Float xyv[6];
};

class Iir2s
{
public:
	Iir2s();
	~Iir2s();

	Iir2s( const Iir2s & inst );
	const Iir2s & operator=( const Iir2s & inst );

	void resize( int sz );
	int size() const;

	const Array & operator()( const Array & v );
	void reset_to( const Array & v );

	void set_a0( Float a0 );
	Float get_a0() const;

	void set_a1( Float a1 );
	Float get_a1() const;

	void set_b0( Float b0 );
	Float get_b0() const;

	void set_b1( Float b1 );
	Float get_b1() const;

	void set_gain( Float gain );
	Float get_gain() const;

private:
	void update_coeffs();

	Float a[2];
	Float b[1];
	Float gain;

	int reset_iters_qty;

	Vector<Iir2> filters;
	Array        ret;
};


class Iir2sRef: public Reference
{
	GDCLASS(Iir2sRef, Reference);
protected:
	static void _bind_methods();

public:
	Iir2sRef();
	~Iir2sRef();

	void resize( int sz );
	int size() const;

	const Array & apply( const Array & v );
	void reset_to( const Array & v );

	void set_a0( real_t a0 );
	real_t get_a0() const;

	void set_a1( real_t a1 );
	real_t get_a1() const;

	void set_b0( real_t b0 );
	real_t get_b0() const;

	void set_b1( real_t b1 );
	real_t get_b1() const;

	void set_gain( real_t gain );
	real_t get_gain() const;

public:
	Iir2s filters;
};

}



#endif

