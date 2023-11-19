
#include "ref_frame_body_node.h"
#include "core/string/print_string.h"
#include "scene/scene_string_names.h"

#include "ref_frame_assembly_node.h"
#include "ref_frame_non_inertial_node.h"

namespace Ign
{

void RefFrameBodyNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_acceleration", "acc"), &RefFrameBodyNode::set_acceleration, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("get_acceleration"),        &RefFrameBodyNode::get_acceleration );

	ClassDB::bind_method( D_METHOD("set_se3_in_physics", "se3"), &RefFrameBodyNode::set_se3_in_physics );
	ClassDB::bind_method( D_METHOD("get_se3_in_physics"),        &RefFrameBodyNode::get_se3_in_physics );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "acceleration" ),  "set_acceleration", "get_acceleration" );
}

RefFrameBodyNode::RefFrameBodyNode()
	: RefFrameNode()
{
}

RefFrameBodyNode::~RefFrameBodyNode()
{
}

void RefFrameBodyNode::set_acceleration( const Vector3 & new_acc )
{
	this->acc = Vector3d( new_acc.x, new_acc.y, new_acc.z );
}

Vector3 RefFrameBodyNode::get_acceleration() const
{
	const Vector3 ret( acc.x_, acc.y_, acc.z_ );
	return ret;
}

void RefFrameBodyNode::set_se3_in_physics( const Ref<Se3Ref> & se3 )
{
	se3_in_physics_ = se3.ptr()->se3;
}

Ref<Se3Ref> RefFrameBodyNode::get_se3_in_physics() const
{
	Ref<Se3Ref> se3;
	se3.instantiate();
	se3.ptr()->se3 = se3_in_physics_;
	return se3;
}

void RefFrameBodyNode::set_se3_in_physics_raw( const SE3 & se3 )
{
	se3_in_physics_ = se3;
}

const SE3 & RefFrameBodyNode::get_se3_in_physics_raw() const
{
	return se3_in_physics_;
}


void RefFrameBodyNode::ign_process( real_t delta )
{
	// The base class' method.
	const int qty = get_child_count();
	SE3 se3_in_physics = get_se3_in_physics_raw();
	SE3 inv_se3_in_physics = se3_in_physics.inverse();
	for ( int i=0; i<qty; i++ )
	{
		Node * ch = get_child( i );
		RefFrameBodyNode * body = Object::cast_to<RefFrameBodyNode>( ch );
		if ( body != nullptr )
		{
			// For each child body compute transform relative to this body.
			SE3 body_se3 = body->get_se3_in_physics_raw();
			body_se3 = inv_se3_in_physics * body_se3;
			body->set_se3_raw( body_se3 );
		}
	}
	RefFrameNode::ign_process( delta );
}

void RefFrameBodyNode::on_parent_jumped()
{
	RefFrameNode::on_parent_jumped();

	// Let all children know recursively.
	const int qty = get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * ch = get_child( i );
		RefFrameNode * rf = Object::cast_to<RefFrameNode>( ch );
		if ( rf != nullptr )
		{
			rf->on_parent_jumped();
		}
	}
}














}



