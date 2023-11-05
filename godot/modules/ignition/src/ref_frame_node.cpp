
#include "ref_frame_node.h"
#include "core/string/print_string.h"
#include "scene/scene_string_names.h"

namespace Ign
{

void RefFrameNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_r", "vector3"),  &RefFrameNode::set_r );
	ClassDB::bind_method( D_METHOD("set_q", "quat"),     &RefFrameNode::set_q );
	ClassDB::bind_method( D_METHOD("set_v", "vector3"),  &RefFrameNode::set_v );
	ClassDB::bind_method( D_METHOD("set_w", "vector3"),  &RefFrameNode::set_w );

	ClassDB::bind_method( D_METHOD("set_t", "transform"),  &RefFrameNode::set_t );
	ClassDB::bind_method( D_METHOD("t"),      &RefFrameNode::t );

	ClassDB::bind_method( D_METHOD("r"),      &RefFrameNode::r );
	ClassDB::bind_method( D_METHOD("q"),      &RefFrameNode::q );
	ClassDB::bind_method( D_METHOD("v"),      &RefFrameNode::v );
	ClassDB::bind_method( D_METHOD("w"),      &RefFrameNode::w );

	ClassDB::bind_method( D_METHOD("set_se3", "se3"), &RefFrameNode::set_se3 );
	GDVIRTUAL_BIND(_set_se3, "se3");
	ClassDB::bind_method( D_METHOD("get_se3"),        &RefFrameNode::get_se3 );

	ClassDB::bind_method( D_METHOD("relative_to", "origin"), &RefFrameNode::relative_to );
	ClassDB::bind_method( D_METHOD("relative_to_se3", "origin", "origin_se3"), &RefFrameNode::relative_to_se3 );
	ClassDB::bind_method( D_METHOD("se3_relative_to", "object_se3", "origin"), &RefFrameNode::se3_relative_to );

	ClassDB::bind_method( D_METHOD("change_parent", "node", "recursive_call"), &RefFrameNode::change_parent );
	GDVIRTUAL_BIND(_change_parent, "node", "recursive_call");

	GDVIRTUAL_BIND(_jumped);
	GDVIRTUAL_BIND(_parent_jumped);
	GDVIRTUAL_BIND(_child_jumped, "child");
	GDVIRTUAL_BIND(_child_entered, "child");
	GDVIRTUAL_BIND(_child_left, "child");

	ClassDB::bind_method( D_METHOD("jump_to", "dest", "dest_se3"), &RefFrameNode::jump_to );

	ClassDB::bind_method( D_METHOD("set_debug", "en"), &RefFrameNode::set_debug );
	ClassDB::bind_method( D_METHOD("get_debug"), &RefFrameNode::get_debug );

	GDVIRTUAL_BIND(_ign_pre_process, "delta");
	GDVIRTUAL_BIND(_ign_process, "delta");
	GDVIRTUAL_BIND(_ign_post_process, "delta");
	GDVIRTUAL_BIND(_ign_physics_pre_process, "delta");
	GDVIRTUAL_BIND(_ign_physics_process, "delta");
	GDVIRTUAL_BIND(_ign_physics_post_process, "delta");

	ClassDB::bind_method( D_METHOD("serialize"),           &RefFrameNode::serialize,   Variant::DICTIONARY );
	GDVIRTUAL_BIND(_serialize, "data");
	ClassDB::bind_method( D_METHOD("deserialize", "data"), &RefFrameNode::deserialize, Variant::BOOL );
	GDVIRTUAL_BIND(_deserialize, "data");

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::TRANSFORM3D, "transform" ),        "set_t", "t" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,     "linear_velocity" ),  "set_v", "v" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3,     "angular_velocity" ), "set_w", "w" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL,        "debug" ), "set_debug", "get_debug" );
	//Otherwise continuously requests "SE3" in editor.
	//ADD_PROPERTY( PropertyInfo( Variant::OBJECT,    "se3" ),              "set_se3", "get_se3" );
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

void RefFrameNode::set_q( const Quaternion & q )
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

void RefFrameNode::set_t( const Transform3D & t )
{
	se3_.set_transform( t );
}

Transform3D RefFrameNode::t() const
{
	const Transform3D res = se3_.transform();
	return res;
}

Vector3 RefFrameNode::r() const
{
	const Vector3 res = se3_.r();
	return res;
}

Quaternion RefFrameNode::q() const
{
	const Quaternion res = se3_.q();
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

void RefFrameNode::set_se3_raw( const SE3 & se3 )
{
	se3_ = se3;
}

void RefFrameNode::set_se3( const Ref<Se3Ref> & se3 )
{
	const SE3 & se3_raw = se3.ptr()->se3;
	set_se3_raw( se3_raw );
	if ( GDVIRTUAL_IS_OVERRIDDEN( _set_se3 ) )
	{
		GDVIRTUAL_CALL( _set_se3, se3 );
	}
}

Ref<Se3Ref> RefFrameNode::get_se3() const
{
	Ref<Se3Ref> se3;
	se3.instantiate();
	se3.ptr()->se3 = se3_;
	return se3;
}


Ref<Se3Ref> RefFrameNode::relative_to( Node * origin )
{
	Ref<Se3Ref> se3;
	se3.instantiate();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf );
	return se3;
}

Ref<Se3Ref> RefFrameNode::relative_to_se3( Node * origin, const Ref<Se3Ref> & origin_se3 )
{
	Ref<Se3Ref> se3;
	se3.instantiate();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf, SE3(), origin_se3->se3 );
	return se3;
}

Ref<Se3Ref> RefFrameNode::se3_relative_to( const Ref<Se3Ref> & object_se3, Node * origin )
{
	Ref<Se3Ref> se3;
	se3.instantiate();

	RefFrameNode * rf = Node::cast_to<RefFrameNode>( origin );
	//if (!rf)
	//	return se3;

	se3->se3 = relative_( rf, object_se3->se3 );
	return se3;
}


void RefFrameNode::change_parent( Node * parent, bool recursive_call )
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
	set_se3_raw( se3_rel );

	if ( parent )
	{
		Node * prev_parent = this->get_parent();
		if ( prev_parent )
			prev_parent->remove_child( this );
		const String unique_name = _unique_name( this->get_name(), parent );
		this->set_name( unique_name );
		parent->add_child( this );
	}

	if ( GDVIRTUAL_IS_OVERRIDDEN( _change_parent ) )
	{
		GDVIRTUAL_CALL( _change_parent, parent, recursive_call );
	}
}

void RefFrameNode::jump_to( Node * dest, const Ref<Se3Ref> & dest_se3 )
{
	jump_to_( dest, dest_se3->se3 );
}

void RefFrameNode::jump_to_( Node * dest, const SE3 & dest_se3 )
{
	const int qty = get_child_count();
	RefFrameNode * dest_rf;
	Node * p = get_parent();
	if ( dest == nullptr )
	{
		dest_rf = nullptr;
	}
	else
	{
		if ( dest != this )
			dest_rf = Object::cast_to<RefFrameNode>( dest );
		else
			dest_rf = Object::cast_to<RefFrameNode>( p );
	}

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
		ch->set_se3_raw( se3_child_to );
	}

	if ( dest != this )
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

	set_se3_raw( dest_se3_adjusted );

	// Call script notifications.
	// For the node itself.
	if (GDVIRTUAL_IS_OVERRIDDEN(_jumped) )
		GDVIRTUAL_CALL(_jumped );
	// And for all the children.
	for ( int i=0; i<qty; i++ )
	{
		Node * n = get_child( i );
		RefFrameNode * ch = Object::cast_to<RefFrameNode>( n );
		if ( ch == nullptr )
			continue;
		ch->on_parent_jumped();
	}

	// Call child jumped in parent.
	if ( p != dest_rf )
	{
		if ( dest_rf != nullptr )
		{
			if (GDVIRTUAL_IS_OVERRIDDEN_PTR(dest_rf, _child_entered) )
				GDVIRTUAL_CALL_PTR(dest_rf, _child_entered, this);
		}
		if ( p != nullptr )
		{
			RefFrameNode * parent_rf = Object::cast_to<RefFrameNode>( p );
			if ( parent_rf != nullptr )
			{
				if (GDVIRTUAL_IS_OVERRIDDEN_PTR(parent_rf, _child_left) )
					GDVIRTUAL_CALL_PTR(parent_rf, _child_left, this);
			}
		}
	}
	else
	{
		if ( dest_rf != nullptr )
		{
			if (GDVIRTUAL_IS_OVERRIDDEN_PTR(dest_rf, _child_jumped) )
				GDVIRTUAL_CALL_PTR(dest_rf, _child_jumped, this);
		}
	}
}

void RefFrameNode::on_parent_jumped()
{
	if (GDVIRTUAL_IS_OVERRIDDEN_PTR(this, _parent_jumped) )
		GDVIRTUAL_CALL_PTR(this, _parent_jumped);
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
		{
			const SE3 & se3 = rf->se3_;
			print_line( String("queueA <- \"") + rf->get_name() + String("\", r: (") +
			            rtos(se3.r_.x_) + String( ", ") +
						rtos(se3.r_.y_) + String( ", ") +
						rtos(se3.r_.z_) + String( "); q: (") +
						rtos(se3.q_.w_) + String( ", ") +
						rtos(se3.q_.x_) + String( ", ") +
						rtos(se3.q_.y_) + String( ", ") +
						rtos(se3.q_.z_) + String( ")")
			);
		}
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
		{
			const SE3 & se3 = rf->se3_;
			print_line( String("queueB <- \"") + rf->get_name() + String("\", r: (") + 
						rtos(se3.r_.x_) + String( ", ") +
						rtos(se3.r_.y_) + String( ", ") +
						rtos(se3.r_.z_) + String( "); q: (") +
						rtos(se3.q_.w_) + String( ", ") +
						rtos(se3.q_.x_) + String( ", ") +
						rtos(se3.q_.y_) + String( ", ") +
						rtos(se3.q_.z_) + String( ")")
			);
		}

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

	if ( debug_ )
	{
		print_line( String("SE3(A): r: (") + 
			rtos(se3A.r_.x_) + String( ", ") +
			rtos(se3A.r_.y_) + String( ", ") +
			rtos(se3A.r_.z_) + String( "); q: (") +
			rtos(se3A.q_.w_) + String( ", ") +
			rtos(se3A.q_.x_) + String( ", ") +
			rtos(se3A.q_.y_) + String( ", ") +
			rtos(se3A.q_.z_) + String( ")")
		);
		print_line( String("SE3(B): r: (") + 
			rtos(se3B.r_.x_) + String( ", ") +
			rtos(se3B.r_.y_) + String( ", ") +
			rtos(se3B.r_.z_) + String( "); q: (") +
			rtos(se3B.q_.w_) + String( ", ") +
			rtos(se3B.q_.x_) + String( ", ") +
			rtos(se3B.q_.y_) + String( ", ") +
			rtos(se3B.q_.z_) + String( ")")
		);
		print_line( String("SE3(A/B): r: (") + 
			rtos(se3_rel.r_.x_) + String( ", ") +
			rtos(se3_rel.r_.y_) + String( ", ") +
			rtos(se3_rel.r_.z_) + String( "); q: (") +
			rtos(se3_rel.q_.w_) + String( ", ") +
			rtos(se3_rel.q_.x_) + String( ", ") +
			rtos(se3_rel.q_.y_) + String( ", ") +
			rtos(se3_rel.q_.z_) + String( ")")
		);
	}

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


void RefFrameNode::ign_pre_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_pre_process) )
	{
		GDVIRTUAL_CALL(_ign_pre_process, delta);
	}
}

void RefFrameNode::ign_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_process) )
	{
		GDVIRTUAL_CALL(_ign_process, delta);
	}
}

void RefFrameNode::ign_post_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_post_process) )
	{
		GDVIRTUAL_CALL(_ign_post_process, delta);
	}
}


void RefFrameNode::ign_physics_pre_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_physics_pre_process) )
	{
		GDVIRTUAL_CALL(_ign_physics_pre_process, delta);
	}

}

void RefFrameNode::ign_physics_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_physics_process) )
	{
		GDVIRTUAL_CALL(_ign_physics_process, delta);
	}

}

void RefFrameNode::ign_physics_post_process( real_t delta )
{
	if ( GDVIRTUAL_IS_OVERRIDDEN(_ign_physics_post_process) )
	{
		GDVIRTUAL_CALL(_ign_physics_post_process, delta);
	}

}


Dictionary RefFrameNode::serialize()
{
	Dictionary data_ret;
	const Dictionary se3_data = se3_.serialize();
	data_ret["se3"] = se3_data;

	if ( GDVIRTUAL_IS_OVERRIDDEN(_serialize) )
	{
		GDVIRTUAL_CALL(_serialize, data_ret);
	}

	return data_ret;
}

bool RefFrameNode::deserialize( const Dictionary & data_arg )
{
	const bool ok = data_arg.has( "se3" );
	if ( !ok )
		return false;

	{
		const Dictionary se3_data = data_arg["se3"];
		const bool ok = se3_.deserialize( se3_data );
		if ( !ok )
			return false;
	}

	if ( GDVIRTUAL_IS_OVERRIDDEN(_deserialize) )
	{
		bool ret;
		GDVIRTUAL_CALL(_deserialize, data_arg, ret );
		return ret;
	}

	return true;
}


}



