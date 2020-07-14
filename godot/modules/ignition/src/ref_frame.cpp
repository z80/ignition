
#include "ref_frame.h"
#include "ref_frame_tree.h"

RefFrame::RefFrame()
	: Reference()
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
    ClassDB::bind_method( D_METHOD("set_rf_name", "string"), &RefFrame::set_name, Variant::NIL );
    ClassDB::bind_method( D_METHOD("rf_name"),               &RefFrame::name, Variant::STRING );

    ClassDB::bind_method( D_METHOD("set_r", "vector3"),   &RefFrame::set_r, Variant::NIL );
	ClassDB::bind_method( D_METHOD("r"),                  &RefFrame::r,    Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "r"), "set_r", "r");

	ClassDB::bind_method( D_METHOD("set_q", "quat"),      &RefFrame::set_q, Variant::NIL );
	ClassDB::bind_method( D_METHOD("q"),                  &RefFrame::q,    Variant::QUAT );
	//ADD_PROPERTY(PropertyInfo(Variant::QUAT, "q"), "set_q", "q");

	ClassDB::bind_method( D_METHOD("set_v", "vector3"),   &RefFrame::set_v, Variant::NIL );
	ClassDB::bind_method( D_METHOD("v"),                  &RefFrame::v,    Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "v"), "set_v", "v");

	ClassDB::bind_method( D_METHOD("set_w", "vector3"),   &RefFrame::set_w, Variant::NIL );
	ClassDB::bind_method( D_METHOD("w"),                  &RefFrame::w,    Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "w"), "set_w", "w");

	ClassDB::bind_method( D_METHOD("set_rf_transform", "transform"), &RefFrame::set_rf_transform, Variant::NIL );
	ClassDB::bind_method( D_METHOD("rf_transform"),                  &RefFrame::rf_transform,    Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("r_root"),             &RefFrame::r_root,    Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("q_root"),             &RefFrame::q_root,    Variant::QUAT );
	ClassDB::bind_method( D_METHOD("v_root"),             &RefFrame::v_root,    Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("w_root"),             &RefFrame::w_root,    Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_origin", "ref_frame"),   &RefFrame::set_origin );
	ClassDB::bind_method( D_METHOD("origin"),                    &RefFrame::origin ); //,    Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("set_root", "ref_frame"), &RefFrame::set_root );
	ClassDB::bind_method( D_METHOD("root"),                  &RefFrame::root ); //,    Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("change_origin", "ref_frame"), &RefFrame::change_origin );
	ClassDB::bind_method( D_METHOD("compute_relative_to_root" ), &RefFrame::compute_relative_to_root );


	ClassDB::bind_method( D_METHOD("set_jump_r", "vector3" ), &RefFrame::set_jump_r );
	ClassDB::bind_method( D_METHOD("set_jump_q", "quat" ),    &RefFrame::set_jump_q );
	ClassDB::bind_method( D_METHOD("set_jump_v", "vector3" ), &RefFrame::set_jump_v );
	ClassDB::bind_method( D_METHOD("set_jump_w", "vector3" ), &RefFrame::set_jump_w );
	ClassDB::bind_method( D_METHOD("set_jump_transform", "transform" ), &RefFrame::set_jump_transform );

	ClassDB::bind_method( D_METHOD("set_obj_r", "vector3" ), &RefFrame::set_obj_r );
	ClassDB::bind_method( D_METHOD("obj_r" ), &RefFrame::obj_r, Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "obj_r"), "set_obj_r", "obj_r");

	ClassDB::bind_method( D_METHOD("set_obj_q", "quat" ),    &RefFrame::set_obj_q );
	ClassDB::bind_method( D_METHOD("obj_q" ), &RefFrame::obj_q, Variant::QUAT );
	//ADD_PROPERTY(PropertyInfo(Variant::QUAT, "obj_q"), "set_obj_q", "obj_q");

	ClassDB::bind_method( D_METHOD("set_obj_v", "vector3" ), &RefFrame::set_obj_v );
	ClassDB::bind_method( D_METHOD("obj_v" ), &RefFrame::obj_v, Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "obj_v"), "set_obj_v", "obj_v");

	ClassDB::bind_method( D_METHOD("set_obj_w", "vector3" ), &RefFrame::set_obj_w );
	ClassDB::bind_method( D_METHOD("obj_w" ), &RefFrame::obj_w, Variant::VECTOR3 );
	//ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "obj_w"), "set_obj_w", "obj_w");


	ClassDB::bind_method( D_METHOD("set_obj_transform", "transform" ), &RefFrame::set_obj_transform );

	ClassDB::bind_method( D_METHOD("calc_obj_jump_state" ), &RefFrame::calc_obj_jump_state );

	ClassDB::bind_method( D_METHOD("obj_transform" ), &RefFrame::obj_transform, Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("apply_jump" ), &RefFrame::apply_jump );
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

void RefFrame::set_rf_transform( const Transform & t )
{
	se3_.set_transform( t );
}

Transform RefFrame::rf_transform() const
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

Vector3 RefFrame::r_root() const
{
	const Vector3 res = se3_rel_to_root_.r();
	return res;
}

Quat    RefFrame::q_root() const
{
	const Quat res = se3_rel_to_root_.q();
	return res;
}

Vector3 RefFrame::v_root() const
{
	const Vector3 res = se3_rel_to_root_.v();
	return res;
}

Vector3 RefFrame::w_root() const
{
	const Vector3 res = se3_rel_to_root_.w();
	return res;
}

void RefFrame::set_origin( Ref<Reference> parent )
{
	if ( parent.is_null() )
	{
		origin_ = -1;
		return;
	}
	RefFrame * rf = Object::cast_to<RefFrame>( parent.ptr() );
	if ( !rf )
	{
		origin_ = -1;
		return;
	}
	origin_ = rf->index_;
}

Ref<Reference> RefFrame::origin() const
{
	if ( tree_.is_null() )
		return nullptr;

	if ( origin_ < 0 )
		return nullptr;

	RefFrame * rf = tree_->frames_.ptr()[origin_];
	return rf;
}

void RefFrame::set_root( Ref<Reference> parent )
{
	if ( parent.is_null() )
	{
		root_ = -1;
		return;
	}
	RefFrame * rf = Object::cast_to<RefFrame>( parent.ptr() );
	if ( !rf )
	{
		root_ = -1;
		return;
	}
	root_ = rf->index_;
}

Ref<Reference> RefFrame::root() const
{
	if ( tree_.is_null() )
		return nullptr;

	if ( root_ < 0 )
		return nullptr;

	RefFrame * rf = tree_->frames_.ptr()[root_];
	return rf;
}

void RefFrame::change_origin( Ref<Reference> origin )
{
	int new_origin;
	if ( origin.is_null() )
	{
		new_origin = -1;
	}
	else
	{
		RefFrame * new_o = Object::cast_to<RefFrame>( origin.ptr() );
		if ( !new_o )
			new_origin = -1;
		else
			new_origin = new_o->index_;
	}

	const SE3 se3_rel = relative( new_origin );
	origin_ = new_origin;
	se3_    = se3_rel;
}

void RefFrame::compute_relative_to_root()
{
	se3_rel_to_root_ = relative( root_ );
}


void RefFrame::set_jump_r( const Vector3 & r )
{
	se3_jump_to_.set_r( r );
}

void RefFrame::set_jump_q( const Quat & q )
{
	se3_jump_to_.set_q( q );
}

void RefFrame::set_jump_v( const Vector3 & v )
{
	se3_jump_to_.set_v( v );
}

void RefFrame::set_jump_w( const Vector3 & w )
{
	se3_jump_to_.set_w( w );
}

void RefFrame::set_jump_transform( const Transform & t )
{
	se3_jump_to_.set_transform( t );
}

void RefFrame::set_obj_r( const Vector3 & r )
{
	se3_obj_cur_.set_r( r );
}

void RefFrame::set_obj_q( const Quat & q )
{
	se3_obj_cur_.set_q( q );
}

void RefFrame::set_obj_v( const Vector3 & v )
{
	se3_obj_cur_.set_v( v );
}

void RefFrame::set_obj_w( const Vector3 & w )
{
	se3_obj_cur_.set_w( w );
}

void RefFrame::set_obj_transform( const Transform & t )
{
	se3_obj_cur_.set_transform( t );
}

void RefFrame::calc_obj_jump_state()
{
	se3_obj_after_jump_ = relative( index_, se3_obj_cur_, se3_jump_to_ );
}

Vector3 RefFrame::obj_r() const
{
	Vector3 res = se3_obj_after_jump_.r();
	return res;
}

Quat    RefFrame::obj_q() const
{
	Quat res = se3_obj_after_jump_.q();
	return res;
}

Vector3 RefFrame::obj_v() const
{
	Vector3 res = se3_obj_after_jump_.v();
	return res;
}

Vector3 RefFrame::obj_w() const
{
	Vector3 res = se3_obj_after_jump_.w();
	return res;
}

Transform RefFrame::obj_transform() const
{
	Transform res = se3_obj_after_jump_.transform();
	return res;
}

void RefFrame::apply_jump()
{
	se3_ = relative( index_, se3_jump_to_ );
}



SE3 RefFrame::relative( int root_ind, const SE3 & se3_local, const SE3 & se3_root )
{
	if ( tree_.is_null() )
		return SE3();

	queueA_.clear();
	RefFrame * rf = this;
	queueA_.push_back( rf );
	int frame_ind = origin_;
	while (frame_ind >= 0)
	{
		rf = tree_->frame(frame_ind);
		queueA_.push_back(rf);
		frame_ind = rf->origin_;
	}
	const int qtyA = queueA_.size();
	int intersection_ind = qtyA;

	queueB_.clear();
	frame_ind = root_ind;
	while ( frame_ind >= 0 )
	{
		rf = tree_->frame(frame_ind);
		// Before adding check if this index is already in "queueA_".
		bool match = false;
		for ( int i = 0; i < qtyA; i++ )
		{
			RefFrame * rfA = queueA_.ptr()[i];
			if ( rfA == rf )
			{
				match = true;
				intersection_ind = i;
				break;
			}
		}
		if ( match )
			break;
		queueB_.push_back( rf );
		frame_ind = rf->origin_;
	}

	// Route A.
	SE3 se3A = se3_local;
	for ( int i = 0; i<intersection_ind; i++ )
	{
		RefFrame * rf = queueA_.ptr()[i];
		const SE3 & se3_rf = rf->se3_;
		se3A = se3_rf * se3A;
	}

	// Route B.
	const int qtyB = queueB_.size();
	SE3 se3B = se3_root;
	for ( int i=0; i<qtyB; i++ )
	{
		const int indB = qtyB - i - 1;
		RefFrame * rf = queueB_.ptr()[indB];
		const SE3 & se3_rf = rf->se3_;
		se3B = se3_rf * se3B;
	}

	const SE3 se3_rel = se3A / se3B;
	return se3_rel;
}









