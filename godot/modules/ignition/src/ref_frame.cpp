
#include "ref_frame.h"
#include "ref_frame_tree.h"

RefFrame::RefFrame()
	: Spatial()
{
	tree_   = nullptr;
	index_  = -1;
	origin_ = -1;
	root_   = -1;
}

RefFrame::~RefFrame()
{
}

void RefFrame::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_name", "string"), &RefFrame::set_name, Variant::NIL );
    ClassDB::bind_method( D_METHOD("name"),               &RefFrame::name, Variant::STRING );

    ClassDB::bind_method( D_METHOD("set_r", "vector3"),   &RefFrame::set_r, Variant::NIL );
	ClassDB::bind_method( D_METHOD("r"),                  &RefFrame::r,    Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("set_q", "quat"),      &RefFrame::set_q, Variant::NIL );
	ClassDB::bind_method( D_METHOD("q"),                  &RefFrame::q,    Variant::QUAT );
	ClassDB::bind_method( D_METHOD("set_v", "vector3"),   &RefFrame::set_v, Variant::NIL );
	ClassDB::bind_method( D_METHOD("v"),                  &RefFrame::v,    Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("set_w", "vector3"),   &RefFrame::set_w, Variant::NIL );
	ClassDB::bind_method( D_METHOD("w"),                  &RefFrame::w,    Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("set_transform", "transform"), &RefFrame::set_transform, Variant::NIL );
	ClassDB::bind_method( D_METHOD("transform"),                  &RefFrame::transform,    Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("set_origin", "ref_frame"), &RefFrame::set_origin );
	ClassDB::bind_method( D_METHOD("origin"),                  &RefFrame::origin ); //,    Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("set_root", "ref_frame"), &RefFrame::set_root );
	ClassDB::bind_method( D_METHOD("root"),                  &RefFrame::root ); //,    Variant::OBJECT );
}

void RefFrame::set_name( const String & name )
{
	name_ = name;
}

const String & RefFrame::name() const
{
	return name_;
}

void RefFrame::set_r( const Vector3 & r )
{
	se3_.set_r( r );
}

void RefFrame::set_q( const Quat & q )
{
	se3_.set_q( q );
}

void RefFrame::set_v( const Vector3 & v )
{
	se3_.set_v( v );
}

void RefFrame::set_w( const Vector3 & w )
{
	se3_.set_w( w );
}

void RefFrame::set_transform( const Transform & t )
{
	const Vector3 r = t.get_origin();
	se3_.set_r( r );
	const Basis b = t.get_basis();
	const Quat q = b.get_quat();
	se3_.set_q( q );
}

Transform RefFrame::transform() const
{
	Transform t;
	t.set_origin( se3_.r() );
	const Quat q = se3_.q();
	const Basis b = q;
	t.set_basis( b );

	return t;
}

Vector3 RefFrame::r() const
{
	const Vector3 res = se3_.r();
	return res;
}

Quat    RefFrame::q() const
{
	const Quat res = se3_.q();
	return res;
}

Vector3 RefFrame::v() const
{
	const Vector3 res = se3_.v();
	return res;
}

Vector3 RefFrame::w() const
{
	const Vector3 res = se3_.w();
	return res;
}

void RefFrame::set_origin( Node * parent )
{
	if ( !parent )
	{
		origin_ = -1;
		return;
	}
	RefFrame * rf = Object::cast_to<RefFrame>( parent );
	if ( !rf )
	{
		origin_ = -1;
		return;
	}
	origin_ = rf->index_;
}

Node * RefFrame::origin() const
{
	if ( !tree_ )
		return nullptr;

	if ( origin_ < 0 )
		return nullptr;

	RefFrame * rf = tree_->frames_.ptr()[origin_];
	return rf;
}

void RefFrame::set_root( Node * parent )
{
	if ( !parent )
	{
		root_ = -1;
		return;
	}
	RefFrame * rf = Object::cast_to<RefFrame>( parent );
	if ( !rf )
	{
		root_ = -1;
		return;
	}
	root_ = rf->index_;
}

Node * RefFrame::root() const
{
	if ( !tree_ )
		return nullptr;

	if ( root_ < 0 )
		return nullptr;

	RefFrame * rf = tree_->frames_.ptr()[root_];
	return rf;
}




