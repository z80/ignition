
#include "ref_frame_non_inertial_node.h"
#include "scene/3d/spatial.h"
#include "save_load.h"


namespace Ign
{

void RefFrameNonInertialNode::_bind_methods()
{
}

RefFrameNonInertialNode::RefFrameNonInertialNode()
	: RefFrameNode()
{
	physics_mode = true;
}

RefFrameNonInertialNode::~RefFrameNonInertialNode()
{
}

void RefFrameNonInertialNode::set_physics_mode( bool en )
{
	physics_mode = en;
}

bool RefFrameNonInertialNode::get_physics_node() const
{
	return physics_mode;
}

void RefFrameNonInertialNode::_ign_pre_process( real_t delta )
{
	RefFrameNode::_ign_pre_process( delta );
}

void RefFrameNonInertialNode::_ign_process( real_t delta )
{
	RefFrameNode::_ign_process( delta );
}

void RefFrameNonInertialNode::_ign_post_process( real_t delta )
{
	RefFrameNode::_ign_post_process( delta );
}

void RefFrameNonInertialNode::_ign_physics_pre_process( real_t delta )
{
}

void RefFrameNonInertialNode::_ign_physics_process( real_t delta )
{
}

void RefFrameNonInertialNode::_ign_physics_post_process( real_t delta )
{
}




}

