
#include "se3_ref.h"

namespace Ign
{

void Se3Ref::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_r", "r"), &Se3Ref::set_r );
	ClassDB::bind_method( D_METHOD("get_r"), &Se3Ref::get_r, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_v", "v"), &Se3Ref::set_v );
	ClassDB::bind_method( D_METHOD("get_v"), &Se3Ref::get_v, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_w", "w"), &Se3Ref::set_w );
	ClassDB::bind_method( D_METHOD("get_w"), &Se3Ref::get_w, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_q", "q"), &Se3Ref::set_q );
	ClassDB::bind_method( D_METHOD("get_q"), &Se3Ref::get_q, Variant::QUAT );

	ClassDB::bind_method( D_METHOD("set_transform", "t"), &Se3Ref::set_transform );
	ClassDB::bind_method( D_METHOD("get_transform"), &Se3Ref::get_transform, Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("copy_from",   "se3"), &Se3Ref::copy_from );
	ClassDB::bind_method( D_METHOD("copy_r_from", "se3"), &Se3Ref::copy_r_from );
	ClassDB::bind_method( D_METHOD("copy_v_from", "se3"), &Se3Ref::copy_v_from );
	ClassDB::bind_method( D_METHOD("copy_w_from", "se3"), &Se3Ref::copy_w_from );
	ClassDB::bind_method( D_METHOD("copy_q_from", "se3"), &Se3Ref::copy_q_from );

	ClassDB::bind_method( D_METHOD("mul", "se3"), &Se3Ref::mul, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("div", "se3"), &Se3Ref::div, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("inverse"),    &Se3Ref::inverse, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("save"),         &Se3Ref::save, Variant::DICTIONARY );
	ClassDB::bind_method( D_METHOD("load", "data"), &Se3Ref::load, Variant::BOOL );

	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "r" ), "set_r", "get_r" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "v" ), "set_v", "get_v" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "w" ), "set_w", "get_w" );
	ADD_PROPERTY( PropertyInfo( Variant::QUAT,      "q" ), "set_q", "get_q" );
	ADD_PROPERTY( PropertyInfo( Variant::TRANSFORM, "transform" ), "set_transform", "get_transform" );
}

Se3Ref::Se3Ref()
{
}

Se3Ref::~Se3Ref()
{
}

void Se3Ref::set_r( const Vector3 & r )
{
	se3.r_ = Vector3d( r.x, r.y, r.z );
}

Vector3 Se3Ref::get_r() const
{
	const Vector3d & r = se3.r_;
	const Vector3 rf( r.x_, r.y_, r.z_ );
	return rf;
}

void Se3Ref::set_v( const Vector3 & v )
{
	se3.v_ = Vector3d( v.x, v.y, v.z );
}

Vector3 Se3Ref::get_v() const
{
	const Vector3d & v = se3.v_;
	const Vector3 vf( v.x_, v.y_, v.z_ );
	return vf;
}

void Se3Ref::set_w( const Vector3 & w )
{
	se3.w_ = Vector3d( w.x, w.y, w.z );
}

Vector3 Se3Ref::get_w() const
{
	const Vector3d & w = se3.w_;
	const Vector3 wf( w.x_, w.y_, w.z_ );
	return wf;
}

void Se3Ref::set_q( const Quat & q )
{
	se3.q_ = Quaterniond( q.w, q.x, q.y, q.z );
}

Quat Se3Ref::get_q() const
{
	const Quaterniond & q = se3.q_;
	const Quat qf( q.x_, q.y_, q.z_, q.w_ );
	return qf;
}

void Se3Ref::set_transform( const Transform & t )
{
	const Vector3 r = t.origin;
	se3.r_ = Vector3d( r.x, r.y, r.z );
	const Quat q = t.basis.get_rotation_quat();
	se3.q_ = Quaterniond( q.w, q.x, q.y, q.z );
}

Transform Se3Ref::get_transform() const
{
	const Vector3 r = Vector3( se3.r_.x_, se3.r_.y_, se3.r_.z_ );
	const Quat    q = Quat( se3.q_.x_, se3.q_.y_, se3.q_.z_, se3.q_.w_ );
	Transform t;
	t.origin = r;
	t.basis = Basis( q );
	return t;
}

void Se3Ref::copy_from( const Ref<Se3Ref> & ref )
{
	se3 = ref.ptr()->se3;
}

void Se3Ref::copy_r_from( const Ref<Se3Ref> & ref )
{
	se3.r_ = ref.ptr()->se3.r_;
}

void Se3Ref::copy_v_from( const Ref<Se3Ref> & ref )
{
	se3.v_ = ref.ptr()->se3.v_;
}

void Se3Ref::copy_w_from( const Ref<Se3Ref> & ref )
{
	se3.w_ = ref.ptr()->se3.w_;
}

void Se3Ref::copy_q_from( const Ref<Se3Ref> & ref )
{
	se3.q_ = ref.ptr()->se3.q_;
}

Ref<Se3Ref> Se3Ref::mul( const Ref<Se3Ref> & rhs )
{
	Ref<Se3Ref> ret;
	ret.instance();
	ret.ptr()->se3 = this->se3 * rhs.ptr()->se3;
	return ret;
}

Ref<Se3Ref> Se3Ref::div( const Ref<Se3Ref> & rhs )
{
	Ref<Se3Ref> ret;
	ret.instance();
	ret.ptr()->se3 = this->se3 / rhs.ptr()->se3;
	return ret;
}

Ref<Se3Ref> Se3Ref::inverse() const
{
	Ref<Se3Ref> ret;
	ret.instance();
	ret.ptr()->se3 = this->se3.inverse();
	return ret;
}

Dictionary Se3Ref::save() const
{
	const Dictionary ret = se3.save();
	return ret;
}

bool Se3Ref::load( const Dictionary & data )
{
	const bool ret = se3.load( data );
	return ret;
}




}



