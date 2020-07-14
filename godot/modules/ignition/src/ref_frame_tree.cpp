
#include "ref_frame_tree.h"
#include "ref_frame.h"

void RefFrameTree::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("push_back", "ref_frame"), &RefFrameTree::push_back );
	ClassDB::bind_method( D_METHOD("clear"),                  &RefFrameTree::clear ); //,    Variant::OBJECT );
}

void RefFrameTree::clear()
{
	const int qty = frames_.size();
	for ( int i=0; i<qty; i++ )
	{
		RefFrame * rf = frames_.ptr()[i];
		rf->index_  = -1;
		rf->origin_ = -1;
		rf->root_   = -1;
		rf->tree_   = nullptr;
	}

	frames_.clear();
}

RefFrameTree::RefFrameTree()
	: Reference()
{
}

RefFrameTree::~RefFrameTree()
{
	clear();
}

void RefFrameTree::push_back( Node * ref_frame )
{
	if ( !ref_frame )
		return;
	RefFrame * rf = Object::cast_to<RefFrame>( ref_frame );
	if ( !rf )
		return;

	const int index = frames_.size();
	frames_.push_back( rf );
	rf->tree_   = Ref<RefFrameTree>( this );
	rf->index_  = index;
	rf->origin_ = -1;
	rf->root_   = -1;
}

RefFrame * RefFrameTree::frame( int index )
{
	RefFrame * rf = frames_.ptr()[index];
	return rf;
}





