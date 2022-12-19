
#ifndef __REF_FRAME_ROOT_H_
#define __REF_FRAME_ROOT_H_

#include "ref_frame_node.h"

namespace Ign
{

class RefFrameRoot: public RefFrameNode
{
	GDCLASS(RefFrameRoot, RefFrameNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameRoot();
	virtual ~RefFrameRoot();

protected:
	void _notification( int p_notification );

	//// Compute forces, integrate dynamics.
	//virtual void _ign_pre_process( real_t delta );
	//// Set positions, place visuals.
	//virtual void _ign_process( real_t delta );
	//// Place camera.
	//virtual void _ign_post_process( real_t delta );

	void _process_children( RefFrameNode * ref_frame, real_t delta );
};

}



#endif





