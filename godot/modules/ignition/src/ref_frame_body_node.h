

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

	// Transform in physics ref. frame.
	void set_se3_in_physics( const Ref<Se3Ref> & se3 );
	Ref<Se3Ref> get_se3_in_physics() const;
	void set_se3_in_physics_raw( const SE3 & se3 );
	const SE3 & get_se3_in_physics_raw() const;

	SE3 se3_in_physics_;

	Node * get_assembly();
	Node * get_ref_frame_physics();

public:
	Vector3d acc;
};

}



#endif





