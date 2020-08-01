
#ifndef __REF_FRAME_TREE_H_
#define __REF_FRAME_TREE_H_

#include "core/reference.h"
#include "core/vector.h"

class RefFrame;

class RefFrameTree: public Reference
{
	GDCLASS(RefFrameTree, Reference);
	OBJ_CATEGORY("Ignition");
protected:
	static void _bind_methods();

public:
	RefFrameTree();
	virtual ~RefFrameTree();

	// On delete NULL all own pointers in all RefFrame objects.
	void clear();
	void push_back( Ref<Reference> ref_frame );
	void remove( Ref<Reference> ref_frame );

	RefFrame * frame( int index );

	Vector< Ref<RefFrame> > frames_;
};



#endif



