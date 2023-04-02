
#include "ref_frame_body_node.h"
#include "core/string/print_string.h"
#include "scene/scene_string_names.h"

namespace Ign
{

void RefFrameBodyNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_acceleration", "acc"), &RefFrameBodyNode::set_acceleration, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("get_acceleration"),        &RefFrameBodyNode::get_acceleration );

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

void RefFrameBodyNode::set_acceleration( const Vector3 & acc )
{
	this->acc = Vector3d( acc.x, acc.y, acc.z );
}

Vector3 RefFrameBodyNode::get_acceleration() const
{
	const Vector3 ret( acc.x_, acc.y_, acc.z_ );
	return ret;
}









}



