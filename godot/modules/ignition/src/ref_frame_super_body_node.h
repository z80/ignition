
#ifndef __REF_FRAME_SUPER_BODY_NODE_H_
#define __REF_FRAME_SUPER_BODY_NODE_H_

#include "ref_frame_body_node.h"

namespace Ign
{

class RefFrameSuperBodyNode: public RefFrameBodyNode
{
	GDCLASS(RefFrameSuperBodyNode, RefFrameBodyNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameSuperBodyNode();
	virtual ~RefFrameSuperBodyNode();

};

}



#endif





