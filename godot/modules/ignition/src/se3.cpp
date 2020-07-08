
#include "se3.h"

namespace Ign
{

SE3::SE3()
{
}

SE3::~SE3()
{
}

SE3::SE3( const SE3 & rhs )
{
	*this = rhs;
}

const SE3 & SE3::operator=( const SE3 & rhs )
{
	if ( this != &rhs )
	{
		q_ = rhs.q_;
		r_ = rhs.r_;
		v_ = rhs.v_;
		w_ = rhs.w_;
	}
	return *this;
}

SE3 SE3::operator*( const SE3 & rhs ) const
{
	const SE3 s = rhs.absolute_to( *this );
	return s;
}

SE3 SE3::operator/( const SE3 & rhs ) const
{
	const SE3 s = this->relative_to( rhs );
	return s;
}


SE3 SE3::relative_to( const SE3 & o ) const
{
	SE3 s;
	const Quaterniond o_qInv = o.q_.Inverse();
	const Vector3d    dr = r_ - o.r_;
	s.q_ = o_qInv * q_;
	s.r_ = o_qInv * dr;
	s.w_ = o_qInv * (w_ - o.w_);
	s.v_ = o_qInv * (v_ - o.v_ - o.w_.CrossProduct( dr ) );

	return s;
}

SE3 SE3::absolute_to( const SE3 & o ) const
{
	SE3 s;
	s.v_ = o.v_ + o.q_*v_ + o.w_.CrossProduct( o.q_*r_ );
	s.w_ = o.w_ + o.q_*w_;
	s.r_ = o.r_ + o.q_*r_;
	s.q_ = o.q_*q_;

	return s;
}


}



