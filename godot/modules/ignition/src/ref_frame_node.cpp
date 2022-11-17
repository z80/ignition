
#include "ref_frame_node.h"
#include "core/print_string.h"
#include "scene/scene_string_names.h"

namespace Ign
{

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

	ClassDB::bind_method( D_METHOD("set_se3"), &RefFrameNode::set_se3 );
	ClassDB::bind_method( D_METHOD("get_se3"), &RefFrameNode::get_se3, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("relative_to", "origin"), &RefFrameNode::relative_to, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("relative_to_se3", "origin", "origin_se3"), &RefFrameNode::relative_to_se3, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("se3_relative_to", "object_se3", "origin"), &RefFrameNode::se3_relative_to, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("change_parent", "node"), &RefFrameNode::change_parent, Variant::NIL );

	ClassDB::bind_method( D_METHOD("compute_relative_to_root", "node"), &RefFrameNode::compute_relative_to_root, Variant::NIL );

	ClassDB::bind_method( D_METHOD("set_jump_r", "vector3"),   &RefFrameNode::set_jump_r, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_q", "quat"),      &RefFrameNode::set_jump_q, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_v", "vector3"),   &RefFrameNode::set_jump_v, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_w", "vector3"),   &RefFrameNode::set_jump_w, Variant::NIL );
	ClassDB::bind_method( D_METHOD("set_jump_t", "transform"), &RefFrameNode::set_jump_t, Variant::NIL );

	ClassDB::bind_method( D_METHOD("apply_jump"),              &RefFrameNode::apply_jump, Variant::NIL );
	ClassDB::bind_method( D_METHOD("jump_to", "dest", "dest_se3"), &RefFrameNode::jump_to );

	ClassDB::bind_method( D_METHOD("set_debug", "en"), &RefFrameNode::set_debug );
	ClassDB::bind_method( D_METHOD("get_debug"), &RefFrameNode::get_debug, Variant::BOOL );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::TRANSFORM, "transform" ),        "set_t", "t" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "linear_velocity" ),  "set_v", "v" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,   "angular_velocity" ), "set_w", "w" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,      "debug" ), "set_debug", "get_debug" );
	//Otherwise continuously requests "SE3" in editor.
	//ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ),      "set_se3", "get_se3" );

	ADD_SIGNAL( MethodInfo( "jumped", PropertyInfo(Variant::OBJECT, "sender") ) );
}



void RefFrameNode::_jumped()
{
	ScriptInstance * si = get_script_instance();
	if ( si != nullptr )
	{
		//const Variant *ptr[1] = {};
		get_script_instance()->call_multilevel( "_jumped", nullptr, 0 );
	}
	if ( debug_ )
		print_line( "jumped" );
}

void RefFrameNode::_parent_jumped()
{
	ScriptInstance * si = get_script_instance();
	if ( si != nullptr )
	{
		//const Variant *ptr[1] = {};
		get_script_instance()->call_multilevel( "_parent_jumped", nullptr, 0 );
	}
	if ( debug_ )
		print_line( "parent jumped" );
}

void RefFrameNode::_child_jumped( RefFrameNode * child_ref_frame )
{
	ScriptInstance * si = get_script_instance();
	if ( si != nullptr )
	{
		const Variant arg( child_ref_frame );
		const Variant *ptr[1] = { &arg };
		get_script_instance()->call_multilevel( "_child_jumped", ptr, 1 );
	}
	if ( debug_ )
		print_line( "child jumped" );
}

RefFrameNode::RefFrameNode()
	: Node(),
	  debug_( false )
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

void RefFrameNode::set_se3( const Ref<Se3Ref> & se3 )
{
	se3_ = se3.ptr()->se3;
}

Ref<Se3Ref> RefFrameNode::get_se3() const
{
	Ref<Se3Ref> se3;
	se3.instance();
	se3.ptr()->se3 = se3_;
	return se3;
}


Ref<Se3Ref> RefFrameNode::relative_to( Node * origin )
{
	Ref<Se3Ref> se3;
	se3.instance();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf );
	return se3;
}

Ref<Se3Ref> RefFrameNode::relative_to_se3( Node * origin, const Ref<Se3Ref> & origin_se3 )
{
	Ref<Se3Ref> se3;
	se3.instance();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf, SE3(), origin_se3->se3 );
	return se3;
}

Ref<Se3Ref> RefFrameNode::se3_relative_to( const Ref<Se3Ref> & object_se3, Node * origin )
{
	Ref<Se3Ref> se3;
	se3.instance();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf, object_se3->se3 );
	return se3;
}


void RefFrameNode::change_parent( Node * parent )
{
	RefFrameNode * parent_rf;
	if ( parent == nullptr )
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
		const String unique_name = _unique_name( this->get_name(), parent );
		this->set_name( unique_name );
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
	jump_to_( this, se3_jump_to_ );
}

void RefFrameNode::jump_to( Node * dest, const Ref<Se3Ref> & dest_se3 )
{
	jump_to_( dest, dest_se3->se3 );
}

void RefFrameNode::jump_to_( Node * destination, const SE3 & dest_se3 )
{
	const int qty = get_child_count();
	Node * p = get_parent();
	Node * dest = ( destination == nullptr ) ? p : destination;
	RefFrameNode * dest_rf = ( dest == nullptr ) ?
									nullptr :
									Object::cast_to<RefFrameNode>( dest );

	SE3 dest_se3_adjusted;
	if ( dest != this )
		dest_se3_adjusted = dest_se3;
	else
	{
		RefFrameNode * parent_rf = Object::cast_to<RefFrameNode>( p );
		dest_se3_adjusted = relative_( parent_rf, dest_se3, SE3() );
	}


	if ( debug_ )
	{
		const SE3 sold = se3_;
		const SE3 snew = dest_se3_adjusted;
		String stri = String( this->get_name() ) + String( ": v0: (" )
			+ rtos( sold.v_.x_ ) + String( ", " ) 
			+ rtos( sold.v_.y_ ) + String( ", " ) 
			+ rtos( sold.v_.z_ ) + String( "), v1: (" ) 
			+ rtos( snew.v_.x_ ) + String( ", " ) 
			+ rtos( snew.v_.y_ ) + String( ", " ) 
			+ rtos( snew.v_.z_ ) + String( ")" );
		print_line( stri );
	}
	for ( int i=0; i<qty; i++ )
	{
		Node * n = get_child( i );
		RefFrameNode * ch = Object::cast_to<RefFrameNode>( n );
		if ( !ch )
			continue;
		const SE3 se3_child_to = ch->relative_( dest_rf, SE3(), dest_se3_adjusted );
		if ( debug_ )
		{
			const SE3 sold = ch->se3_;
			const SE3 snew = se3_child_to;
			String stri = String( "    " ) + String( ch->get_name() ) + String( ": v0: (" )
				          + rtos( sold.v_.x_ ) + String( ", " ) 
				          + rtos( sold.v_.y_ ) + String( ", " ) 
						  + rtos( sold.v_.z_ ) + String( "), v1: (" ) 
						  + rtos( snew.v_.x_ ) + String( ", " ) 
				          + rtos( snew.v_.y_ ) + String( ", " ) 
				          + rtos( snew.v_.z_ ) + String( ")" );
			print_line( stri );
		}
		ch->se3_ = se3_child_to;
	}

	if ( (dest != this) && (p != dest) )
	{
		if ( p != nullptr )
		{
			RefFrameNode * parent_rf = Object::cast_to<RefFrameNode>( p );
			p->remove_child( this );
		}

		if ( dest )
		{
			dest->add_child( this );
		}
	}

	se3_ = dest_se3_adjusted;

	// Call script notifications.
	// For the node itself.
	_jumped();
	// And for all the children.
	for ( int i=0; i<qty; i++ )
	{
		Node * n = get_child( i );
		RefFrameNode * ch = Object::cast_to<RefFrameNode>( n );
		if ( !ch )
			continue;
		ch->_parent_jumped();
	}

	// Call child jumped in parent.
	if ( dest_rf != nullptr )
		dest_rf->_child_jumped( this );
}


SE3 RefFrameNode::relative_( RefFrameNode * root, const SE3 & se3_local, const SE3 & se3_root )
{
	if ( debug_ && (root != nullptr) )
	{
		print_line( String("relative \"") + this->get_name() + String( "\" relative to \"" ) + root->get_name() + String( "\"" ) );
		print_line( String("own  path: ") + this->get_path() );
		print_line( String("root path: ") + root->get_path() );
	}

	queueA_.clear();
	RefFrameNode * rf = this;
	while (rf != nullptr)
	{
		if ( debug_ )
			print_line( String("queueA <- \"") + rf->get_name() + String("\"") );
		queueA_.push_back( rf );
		rf = rf->parent_rf_();
	}
	const int qtyA = queueA_.size();
	int intersection_ind = qtyA;

	if ( debug_ )
	{
		print_line( String( "intersection_ind <- " ) + itos( intersection_ind ) );
		print_line( String("---") );
	}

	queueB_.clear();
	rf = root;
	while ( rf != nullptr )
	{
		// Before adding check if this index is already in "queueA_".
		const int index_in_a = queueA_.find( rf );
		const bool match = (index_in_a >= 0);
		if ( match )
		{
			intersection_ind = index_in_a;
			break;
		}
		if ( debug_ )
			print_line( String("queueB <- \"") + rf->get_name() + String("\"") );
		queueB_.push_back( rf );
		rf = rf->parent_rf_();
	}

	if ( debug_ )
	{
		print_line( String( "intersection_ind <- " ) + itos( intersection_ind ) );
		print_line( String("---") );
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
		RefFrameNode * rf = queueB_.ptr()[i];
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

void RefFrameNode::set_debug( bool en )
{
	debug_ = en;
}

bool RefFrameNode::get_debug() const
{
	return debug_;
}

String RefFrameNode::_unique_name( const String & name_base, Node * parent )
{
	if (parent == nullptr)
		return name_base;
	const NodePath parent_path = parent->get_path();
	const String s_parent_path = String(parent_path);
	const String path = s_parent_path + String( "/" ) + name_base;
	Node * n = parent->get_node_or_null( path );
	if (n == nullptr)
		return name_base;
	int ind = 0;
	while (true)
	{
		const String name = name_base + String("_") + rtos( ind );
		const String path = s_parent_path + String("/") + name;
		Node * n = parent->get_node_or_null( path );
		if ( n == nullptr )
			return name;
		ind += 1;
	}
}


}



