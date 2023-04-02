
#ifndef __REF_FRAME_ROOT_H_
#define __REF_FRAME_ROOT_H_

#include "ref_frame_node.h"

namespace Ign
{

class RefFrameRoot: public RefFrameNode
{
	GDCLASS(RefFrameRoot, RefFrameNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameRoot();
	virtual ~RefFrameRoot();

	void set_time_dilation( real_t gain );
	real_t get_time_dilation() const;

protected:
	void _notification( int p_notification );

	void _pre_process_children( RefFrameNode * ref_frame, real_t delta );
	void _process_children( RefFrameNode * ref_frame, real_t delta );
	void _post_process_children( RefFrameNode * ref_frame, real_t delta );

	void _physics_pre_process_children( RefFrameNode * ref_frame, real_t delta );
	void _physics_process_children( RefFrameNode * ref_frame, real_t delta );
	void _physics_post_process_children( RefFrameNode * ref_frame, real_t delta );

public:
	Float time_dilation;
};

}



#endif





