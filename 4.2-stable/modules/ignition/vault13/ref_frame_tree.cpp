
#include "ref_frame_tree.h"
#include "ref_frame.h"

void RefFrameTree::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("push_back", "ref_frame"), &RefFrameTree::push_back );
	ClassDB::bind_method( D_METHOD("clear"),                  &RefFrameTree::clear ); //,    Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("remove", "ref_frame"),    &RefFrameTree::remove );
}

void RefFrameTree::clear()
{
	const int qty = frames_.size();
	for ( int i=0; i<qty; i++ )
	{
		Ref<RefFrame> rf = frames_.ptr()[i];
		rf->index_  = -1;
		rf->origin_ = -1;
		rf->root_   = -1;
		rf->tree_   = nullptr;
	}

	frames_.clear();
}

RefFrameTree::RefFrameTree()
	: RefCounted()
{
}

RefFrameTree::~RefFrameTree()
{
	clear();
}

void RefFrameTree::push_back( Ref<RefCounted> ref_frame )
{
	if ( ref_frame.is_null() )
		return;
	RefFrame * rf = Object::cast_to<RefFrame>( ref_frame.ptr() );
	if ( !rf )
		return;

	const int index = frames_.size();
	frames_.push_back( rf );
	rf->tree_   = Ref<RefFrameTree>( this );
	rf->index_  = index;
	rf->origin_ = -1;
	rf->root_   = -1;
}

void RefFrameTree::remove( Ref<RefCounted> ref_frame )
{
	if ( ref_frame.is_null() )
		return;
	RefFrame * rf = Object::cast_to<RefFrame>( ref_frame.ptr() );
	if ( !rf )
		return;

	const int ind = rf->index_;
	if ( ind < 0 )
		return;
	Ref<RefCounted> origin = rf->origin();
	const int origin_ind = rf->origin_;

	const int qty = frames_.size();

	// First reassign root for children.
	SE3 rel;
	for ( int i=0; i<qty; i++ )
	{
		Ref<RefFrame> crf = frames_.ptrw()[i];
		if ( crf->origin_ == ind )
		{
			rel = crf->relative( origin_ind );
			crf->change_origin( origin );
		}
	}

	// Now shift all entries by one and reassign children.
	for ( int i=ind+1; i<qty; i++ )
	{
		Ref<RefFrame> shifted_rf = frames_.ptrw()[i];
		const int new_ind = i-1;
		frames_.ptrw()[new_ind] = shifted_rf;
		for ( int j=0; j<qty; j++ )
		{
			if ( ( j == i ) || (j == new_ind) )
				continue;
			Ref<RefFrame> child_rf = frames_.ptrw()[j];
			if ( child_rf->origin_ == i )
				child_rf->origin_ = new_ind;
			if ( child_rf->root_ == i )
				child_rf->root_ = new_ind;
		}
	}

	// Now resize the array.
	frames_.resize( qty-1 );
}

RefFrame * RefFrameTree::frame( int index )
{
	RefFrame * rf = frames_.ptrw()[index].ptr();
	return rf;
}





