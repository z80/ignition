
#ifndef __REF_FRAME_ASSEMBLY_NODE_H_
#define __REF_FRAME_ASSEMBLY_NODE_H_

#include "ref_frame_body_node.h"

namespace Ign
{

class RefFrameAssemblyNode: public RefFrameBodyNode
{
	GDCLASS(RefFrameAssemblyNode, RefFrameBodyNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameAssemblyNode();
	virtual ~RefFrameAssemblyNode();

	virtual void ign_process( real_t delta ) override;

};

}



#endif





