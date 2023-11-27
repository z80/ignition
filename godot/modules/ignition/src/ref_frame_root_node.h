
#ifndef __REF_FRAME_ROOT_NODE_H_
#define __REF_FRAME_ROOT_NODE_H_

#include "ref_frame_node.h"

namespace Ign
{

class RefFrameRootNode: public RefFrameNode
{
	GDCLASS(RefFrameRootNode, RefFrameNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameRootNode();
	virtual ~RefFrameRootNode();

	void set_time_dilation( real_t gain );
	real_t get_time_dilation() const;

	void set_camera_node( Node * node );
	Node * get_camera_node() const;

protected:
	void _notification( int p_notification );

	void pre_process_children( RefFrameNode * ref_frame, real_t delta );
	void process_children( RefFrameNode * ref_frame, real_t delta );
	void post_process_children( RefFrameNode * ref_frame, real_t delta );

	void physics_pre_process_children( RefFrameNode * ref_frame, real_t delta );
	void physics_process_children( RefFrameNode * ref_frame, real_t delta );
	void physics_post_process_children( RefFrameNode * ref_frame, real_t delta );

public:
	Float time_dilation;
	RefFrameNode * camera_node_;
};

}



#endif





