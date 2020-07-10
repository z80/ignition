
#ifndef __REF_FRAME_TREE_H_
#define __REF_FRAME_TREE_H_

#include "core/reference.h"
#include "core/vector.h"

class RefFrame;

class RefFrameTree: public Reference
{
protected:
	void _bind_methods();

	// On delete NULL all own pointers in all RefFrame objects.
	void cleanup();
public:
	RefFrameTree();
	virtual ~RefFrameTree();

	void add_ref_frame( Node * ref_frame );
	void remove_ref_frame( Node * ref_frame );

	Vector< RefFrame * > frames_;
};



#endif



