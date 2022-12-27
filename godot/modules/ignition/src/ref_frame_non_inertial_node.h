
#ifndef __REF_FRAME_NON_INERTIAL_NODE_H_
#define __REF_FRAME_NON_INERTIAL_NODE_H_

#include "ref_frame_motion_node.h"
#include "celestial_motion.h"
#include "distance_scaler_base_ref.h"

namespace Ign
{

class RefFrameNonInertialNode: public RefFrameMotionNode
{
	GDCLASS(RefFrameNonInertialNode, RefFrameMotionNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameNonInertialNode();
	virtual ~RefFrameNonInertialNode();

	virtual void _ign_pre_process( real_t delta ) override;
	virtual void _ign_process( real_t delta ) override;
	virtual void _ign_post_process( real_t delta ) override;
};


}




#endif


