

#ifndef __REF_FRAME_STATIC_NODE_H_
#define __REF_FRAME_STATIC_NODE_H_

#include "ref_frame_node.h"

namespace Ign
{

class RefFrameStaticNode: public RefFrameNode
{
	GDCLASS(RefFrameStaticNode, RefFrameNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameStaticNode();
	virtual ~RefFrameStaticNode();
};

}



#endif





