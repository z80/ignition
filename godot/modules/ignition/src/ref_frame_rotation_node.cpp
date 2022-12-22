
#include "ref_frame_rotation_node.h"
#include "save_load.h"


namespace Ign
{

void RefFrameRotationNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("init", "up", "period_hrs"), &RefFrameRotationNode::init );
	ClassDB::bind_method( D_METHOD("serialize"),                &RefFrameRotationNode::serialize, Variant::DICTIONARY );
	ClassDB::bind_method( D_METHOD("deserialize", "arg"),       &RefFrameRotationNode::deserialize );
}

RefFrameRotationNode::RefFrameRotationNode()
	: RefFrameNode()
{
}

RefFrameRotationNode::~RefFrameRotationNode()
{
}

void RefFrameRotationNode::init( const Vector3 & up, real_t period_hrs )
{
	celestial_rotation.init( Vector3d( up.x, up.y, up.z ), period_hrs );
}

Dictionary RefFrameRotationNode::serialize() const
{
	Dictionary data;
	data["spinning"] = celestial_rotation.spinning;
	data["period"]   = celestial_rotation.period;
	data["time"]     = celestial_rotation.time;
	serialize_quat( celestial_rotation.axis_orientation, "axis_orientation", data );
	data["se3"] = celestial_rotation.se3.serialize();

	return data;
}

bool RefFrameRotationNode::deserialize( const Dictionary & data )
{
	celestial_rotation.spinning = data["spinning"];
	celestial_rotation.period = data["period"];
	celestial_rotation.time = data["time"];
	celestial_rotation.axis_orientation = deserialize_quat( "axis_orientation", data );
	celestial_rotation.se3.deserialize( data["se3"] );

	return true;
}

void RefFrameRotationNode::_ign_pre_process( real_t delta )
{
	this->se3_ = celestial_rotation.process( delta );
}

void RefFrameRotationNode::_ign_process( real_t delta )
{
}

void RefFrameRotationNode::_ign_post_process( real_t delta )
{
}



}

