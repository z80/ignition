
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

	ClassDB::bind_method( D_METHOD("get_assembly"),          &RefFrameBodyNode::get_assembly );
	ClassDB::bind_method( D_METHOD("get_ref_frame_physics"), &RefFrameBodyNode::get_ref_frame_physics );

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

Node * RefFrameBodyNode::get_assembly()
{
	Node * p = get_parent();
	if ( p == nullptr )
		return nullptr;
	RefFrameAssemblyNode * a = Object::cast_to<RefFrameAssemblyNode>( p );
	return a;
}

Node * RefFrameBodyNode::get_ref_frame_physics()
{
	Node * p = get_parent();
	if ( p == nullptr )
		return nullptr;

	RefFrameNonInertialNode * a = Object::cast_to<RefFrameNonInertialNode>( p );
	if ( a != nullptr )
		return a;

	p = p->get_parent();
	if ( p == nullptr )
		return nullptr;

	a = Object::cast_to<RefFrameNonInertialNode>( p );
	return a;
}














}



