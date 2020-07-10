
#include "se3.h"

namespace Ign
{

SE3::SE3()
{
}

SE3::~SE3()
{
}

SE3::SE3( const Vector3d & r, const Quaterniond & q, const Vector3d & v, const Vector3d & w )
{
	r_ = r;
	q_ = q;
	v_ = v;
	w_ = w;
}

SE3::SE3( const Vector3 & r, const Quat & q, const Vector3 & v, const Vector3 & w )
{
	r_.x_ = r.x;
	r_.y_ = r.y;
	r_.z_ = r.z;

	q_.w_ = q.w;
	q_.x_ = q.x;
	q_.y_ = q.y;
	q_.z_ = q.z;

	v_.x_ = v.x;
	v_.y_ = v.y;
	v_.z_ = v.z;

	w_.x_ = w.x;
	w_.y_ = w.y;
	w_.z_ = w.z;
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

void SE3::set_r( const Vector3 & r )
{
	r_.x_ = r.x;
	r_.y_ = r.y;
	r_.z_ = r.z;
}

void SE3::set_q( const Quat & q )
{
	q_.w_ = q.w;
	q_.x_ = q.x;
	q_.y_ = q.y;
	q_.z_ = q.z;
}

void SE3::set_v( const Vector3 & v )
{
	v_.x_ = v.x;
	v_.y_ = v.y;
	v_.z_ = v.z;
}

void SE3::set_w( const Vector3 & w )
{
	w_.x_ = w.x;
	w_.y_ = w.y;
	w_.z_ = w.z;
}

Vector3 SE3::r() const
{
	const Vector3 res( r_.x_, r_.y_, r_.z_ );
	return res;
}

Quat    SE3::q() const
{
	const Quat res( q_.x_, q_.y_, q_.z_, q_.w_ );
	return res;
}

Vector3 SE3::v() const
{
	const Vector3 res( v_.x_, v_.y_, v_.z_ );
	return res;
}

Vector3 SE3::w() const
{
	const Vector3 res( w_.x_, w_.y_, w_.z_ );
	return res;
}



}



