
#include "ref_frame_non_inertial_node.h"
#include "scene/3d/spatial.h"
#include "save_load.h"


namespace Ign
{

void RefFrameNonInertialNode::_bind_methods()
{
}

RefFrameNonInertialNode::RefFrameNonInertialNode()
	: RefFrameMotionNode()
{
}

RefFrameNonInertialNode::~RefFrameNonInertialNode()
{
}


void RefFrameNonInertialNode::_ign_pre_process( real_t delta )
{
	RefFrameMotionNode::_ign_pre_process( delta );
}

void RefFrameNonInertialNode::_ign_process( real_t delta )
{
	RefFrameMotionNode::_ign_process( delta );
}

void RefFrameNonInertialNode::_ign_post_process( real_t delta )
{
	RefFrameMotionNode::_ign_post_process( delta );
}


}

