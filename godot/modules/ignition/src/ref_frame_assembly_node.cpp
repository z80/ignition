
#include "ref_frame_assembly_node.h"
#include "core/string/print_string.h"
#include "scene/scene_string_names.h"

namespace Ign
{

void RefFrameAssemblyNode::_bind_methods()
{
}

RefFrameAssemblyNode::RefFrameAssemblyNode()
	: RefFrameBodyNode()
{
}

RefFrameAssemblyNode::~RefFrameAssemblyNode()
{
}

void RefFrameAssemblyNode::ign_process( real_t delta )
{
	// The base class' method.
	const int qty = get_child_count();
	bool applied = false;
	for ( int i=0; i<qty; i++ )
	{
		Node * ch = get_child( 0 );
		RefFrameBodyNode * body = Object::cast_to<RefFrameBodyNode>( ch );
		if ( body != nullptr )
		{
			if ( !applied )
			{
				applied = false;
			}
		}
	}
	RefFrameNode::ign_process( delta );
}







}



