
#include "numerical_motion.h"

#include "core/variant.h"
#include "core/dictionary.h"
#include "core/print_string.h"

namespace Ign
{



struct RK4_Vector6
{
	Float x[6];
};

static void rk4_f_acc( const RK4_Vector6 & x, const Vector3d & acc, Float h, RK4_Vector6 & f )
{
	const Vector3d r( x.x[0], x.x[1], x.x[2] );
	const Float abs_r = r.Length();
	// Position derivative is velocity.
	f.x[0] = h * x.x[3];
	f.x[1] = h * x.x[4];
	f.x[2] = h * x.x[5];
	// Velocity derivative is acceleration.
	f.x[3] = h * acc.x_;
	f.x[4] = h * acc.y_;
	f.x[5] = h * acc.z_;
}

static void rk4_step_acc( SE3 & se3, const Vector3d & acc, Float h )
{
	RK4_Vector6 x, x1, x2, x3, k1, k2, k3, k4;
	x.x[0] = se3.r_.x_;
	x.x[1] = se3.r_.y_;
	x.x[2] = se3.r_.z_;
	x.x[3] = se3.v_.x_;
	x.x[4] = se3.v_.y_;
	x.x[5] = se3.v_.z_;

	rk4_f_acc( x, acc, h, k1 );
	for ( int i=0; i<6; i++ )
		x1.x[i] = x.x[i] + k1.x[i]*0.5;

	rk4_f_acc( x1, acc, h, k2 );
	for ( int i=0; i<6; i++ )
		x2.x[i] = x.x[i] + k2.x[i]*0.5;

	rk4_f_acc( x2, acc, h, k3 );
	for ( int i=0; i<6; i++ )
		x3.x[i] = x.x[i] + k3.x[i]*0.5;

	rk4_f_acc( x3, acc, h, k4 );

	for ( int i=0; i<6; i++ )
	{
		const Float dx_i = (k1.x[0] + 2.0*k2.x[i] + 2.0*k3.x[i] + k4.x[i]) / 6.0;
		x.x[i] += dx_i;
	}

	se3.r_.x_ = x.x[0];
	se3.r_.y_ = x.x[1];
	se3.r_.z_ = x.x[2];
	se3.v_.x_ = x.x[3];
	se3.v_.y_ = x.x[4];
	se3.v_.z_ = x.x[5];
}


NumericalMotion::NumericalMotion()
	: debug( false ),
	  time_step( 10.0 )
{
}

NumericalMotion::~NumericalMotion()
{
}

void NumericalMotion::process( SE3 & se3, Float dt, const Vector3d & acc )
{
	Float T = dt;
	while (T > 0.0)
	{
		Float delta;
		if ( T > time_step )
		{
			delta = time_step;
			T    -= time_step;
		}
		else
		{
			delta = T;
			T     = 0.0;
		}
		rk4_step_acc( se3, acc, delta );
	}

	if (debug)
	{
		print_line( String("numerical integration acc: x: (" ) +
			rtos(se3.r_.x_) + String(", ") +
			rtos(se3.r_.y_) + String(", ") +
			rtos(se3.r_.z_) + String(")") );
	}
}


Dictionary NumericalMotion::serialize() const
{
	Dictionary data;
	data["time_step"] = time_step;

	return data;
}

bool NumericalMotion::deserialize( const Dictionary & data )
{
	time_step = data["time_step"];
	return true;
}



}







