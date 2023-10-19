

#ifndef __REF_FRAME_BODY_NODE_H_
#define __REF_FRAME_BODY_NODE_H_

#include "ref_frame_node.h"

namespace Ign
{

class RefFrameBodyNode: public RefFrameNode
{
	GDCLASS(RefFrameBodyNode, RefFrameNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameBodyNode();
	virtual ~RefFrameBodyNode();

	void set_acceleration( const Vector3 & acc );
	Vector3 get_acceleration() const;

public:
	Vector3d acc;
};

}



#endif





