
#include "ref_frame_node.h"

void RefFrameNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_r", "vector3"),  &RefFrameNode::set_r, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_q", "quat"),  &RefFrameNode::set_q, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_v", "vector3"),  &RefFrameNode::set_v, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_w", "vector3"),  &RefFrameNode::set_w, Variant::NIL );

	ClassDB::bind_method( D_METHOD("set_t", "transform"),  &RefFrameNode::set_t, Variant::NIL );
	ClassDB::bind_method( D_METHOD("t"),      &RefFrameNode::t, Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("r"),      &RefFrameNode::r, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("q"),      &RefFrameNode::q, Variant::QUAT );
	ClassDB::bind_method( D_METHOD("v"),      &RefFrameNode::v, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("w"),      &RefFrameNode::w, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("r_root"), &RefFrameNode::r_root, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("q_root"), &RefFrameNode::q_root, Variant::QUAT );
	ClassDB::bind_method( D_METHOD("v_root"), &RefFrameNode::v_root, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("w_root"), &RefFrameNode::w_root, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("t_root"), &RefFrameNode::t_root, Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("change_parent", "node"), &RefFrameNode::change_parent, Variant::NIL );

	ClassDB::bind_method( D_METHOD("compute_relative_to_root", "node"), &RefFrameNode::compute_relative_to_root, Variant::NIL );

	ClassDB::bind_method( D_METHOD("set_jump_r", "vector3"),   &RefFrameNode::set_jump_r, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_q", "quat"),      &RefFrameNode::set_jump_q, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_v", "vector3"),   &RefFrameNode::set_jump_v, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_w", "vector3"),   &RefFrameNode::set_jump_w, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_t", "transform"), &RefFrameNode::set_jump_t, Variant::NIL );

	ClassDB::bind_method( D_METHOD("apply_jump"),              &RefFrameNode::apply_jump, Variant::NIL );
}

RefFrameNode::RefFrameNode()
	: Node()
{
}

RefFrameNode::~RefFrameNode()
{
}

void RefFrameNode::set_r( const Vector3 & r )
{
	se3_.set_r( r );
}

void RefFrameNode::set_q( const Quat & q )
{
	se3_.set_q( q );
}

void RefFrameNode::set_v( const Vector3 & v )
{
	se3_.set_v( v );
}

void RefFrameNode::set_w( const Vector3 & w )
{
	se3_.set_w( w );
}

void RefFrameNode::set_t( const Transform & t )
{
	se3_.set_transform( t );
}

Transform RefFrameNode::t() const
{
	const Transform res = se3_.transform();
	return res;
}

Vector3 RefFrameNode::r() const
{
	const Vector3 res = se3_.r();
	return res;
}

Quat    RefFrameNode::q() const
{
	const Quat res = se3_.q();
	return res;
}

Vector3 RefFrameNode::v() const
{
	const Vector3 res = se3_.v();
	return res;
}

Vector3 RefFrameNode::w() const
{
	const Vector3 w = se3_.w();
	return w;
}

Vector3 RefFrameNode::r_root() const
{
	const Vector3 res = se3_root_.r();
	return res;
}

Quat    RefFrameNode::q_root() const
{
	const Quat res = se3_root_.q();
	return res;
}

Vector3 RefFrameNode::v_root() const
{
	const Vector3 res = se3_root_.v();
	return res;
}

Vector3 RefFrameNode::w_root() const
{
	const Vector3 res = se3_root_.w();
	return res;
}

Transform RefFrameNode::t_root() const
{
	const Transform res = se3_root_.transform();
	return res;
}

void RefFrameNode::change_parent( Node * parent )
{
	RefFrameNode * parent_rf;
	if ( !parent )
	{
		parent_rf = nullptr;
	}
	else
	{
		RefFrameNode * new_o = Object::cast_to<RefFrameNode>( parent );
		if ( !new_o )
			parent_rf = nullptr;
		else
			parent_rf = new_o;
	}

	const SE3 se3_rel = relative_( parent_rf );
	se3_    = se3_rel;

	if ( parent )
	{
		Node * prev_parent = this->get_parent();
		if ( prev_parent )
			prev_parent->remove_child( this );
		parent->add_child( this );
	}
}

void RefFrameNode::compute_relative_to_root( Node * root )
{
	RefFrameNode * root_rf;
	if ( !root )
	{
		root_rf = nullptr;
	}
	else
	{
		RefFrameNode * new_r = Object::cast_to<RefFrameNode>( root );
		if ( !new_r )
			root_rf = nullptr;
		else
			root_rf = new_r;
	}
	se3_root_ = relative_( root_rf );
}

void RefFrameNode::set_jump_r( const Vector3 & r )
{
	se3_jump_to_.set_r( r );
}

void RefFrameNode::set_jump_q( const Quat & q )
{
	se3_jump_to_.set_q( q );
}

void RefFrameNode::set_jump_v( const Vector3 & v )
{
	se3_jump_to_.set_v( v );
}

void RefFrameNode::set_jump_w( const Vector3 & w )
{
	se3_jump_to_.set_w( w );
}

void RefFrameNode::set_jump_t( const Transform & t )
{
	se3_jump_to_.set_transform( t );
}

void RefFrameNode::apply_jump()
{
	const int qty = get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = get_child( i );
		RefFrameNode * ch = Object::cast_to<RefFrameNode>( n );
		if ( !ch )
			continue;
		const SE3 se3_child_to = ch->relative_( this, SE3(), se3_jump_to_ );
		ch->se3_ = se3_child_to;
	}

	Node * p = get_parent();
	RefFrameNode * parent_rf;
	if ( !p )
		parent_rf = nullptr;
	else
		parent_rf = Object::cast_to<RefFrameNode>( p );

	const SE3 se3_in_parent = relative_( parent_rf, se3_jump_to_ );
	se3_ = se3_in_parent;
}

SE3 RefFrameNode::relative_( RefFrameNode * root, const SE3 & se3_local, const SE3 & se3_root )
{
	queueA_.clear();
	RefFrameNode * rf = this;
	while (rf != nullptr)
	{
		queueA_.push_back( rf );
		rf = rf->parent_rf_();
	}
	const int qtyA = queueA_.size();
	int intersection_ind = qtyA;

	queueB_.clear();
	rf = root;
	while ( rf != nullptr )
	{
		// Before adding check if this index is already in "queueA_".
		bool match = false;
		for ( int i = 0; i < qtyA; i++ )
		{
			RefFrameNode * rfA = queueA_.ptr()[i];
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
		rf = rf->parent_rf_();
	}

	// Route A.
	SE3 se3A = se3_local;
	for ( int i=0; i<intersection_ind; i++ )
	{
		RefFrameNode * rf = queueA_.ptr()[i];
		const SE3 & se3_rf = rf->se3_;
		se3A = se3_rf * se3A;
	}

	// Route B.
	const int qtyB = queueB_.size();
	SE3 se3B = se3_root;
	for ( int i=0; i<qtyB; i++ )
	{
		const int indB = qtyB - i - 1;
		RefFrameNode * rf = queueB_.ptr()[indB];
		const SE3 & se3_rf = rf->se3_;
		se3B = se3_rf * se3B;
	}

	const SE3 se3_rel = se3A / se3B;
	return se3_rel;
}

RefFrameNode * RefFrameNode::parent_rf_() const
{
	Node * n = get_parent();
	if ( !n )
		return nullptr;

	RefFrameNode * p = Object::cast_to<RefFrameNode>( n );
	return p;
}



