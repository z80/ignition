
#include "ref_frame_rotation_node.h"
#include "save_load.h"


namespace Ign
{

void RefFrameRotationNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("init", "up", "period_hrs"), &RefFrameRotationNode::init );
}

RefFrameRotationNode::RefFrameRotationNode()
	: RefFrameNode()
{
}

RefFrameRotationNode::~RefFrameRotationNode()
{
}

void RefFrameRotationNode::set_se3_raw( const SE3 & se3 )
{
	RefFrameNode::set_se3_raw( se3 );
}

void RefFrameRotationNode::init( const Vector3 & up, real_t period_hrs )
{
	cr.init( Vector3d( up.x, up.y, up.z ), period_hrs );
}

Dictionary RefFrameRotationNode::serialize()
{
	Dictionary data;
	data["spinning"] = cr.spinning;
	data["period"]   = cr.period;
	data["time"]     = cr.time;
	serialize_quat( cr.axis_orientation, "axis_orientation", data );
	data["se3"] = cr.se3.serialize();

	return data;
}

bool RefFrameRotationNode::deserialize( const Dictionary & data )
{
	cr.spinning = data["spinning"];
	cr.period = data["period"];
	cr.time = data["time"];
	cr.axis_orientation = deserialize_quat( "axis_orientation", data );
	cr.se3.deserialize( data["se3"] );

	return true;
}

void RefFrameRotationNode::_ign_physics_pre_process( real_t delta )
{
	this->se3_ = cr.process( delta );
	RefFrameNode::_ign_physics_pre_process( delta );
}

void RefFrameRotationNode::_ign_physics_process( real_t delta )
{
	RefFrameNode::_ign_physics_process( delta );
}

void RefFrameRotationNode::_ign_physics_post_process( real_t delta )
{
	RefFrameNode::_ign_physics_post_process( delta );
}



}

