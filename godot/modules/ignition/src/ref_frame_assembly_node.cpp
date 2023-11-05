
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
	SE3 inv_se3_in_physics;
	for ( int i=0; i<qty; i++ )
	{
		Node * ch = get_child( 0 );
		RefFrameBodyNode * body = Object::cast_to<RefFrameBodyNode>( ch );
		if ( body != nullptr )
		{
			if ( !applied )
			{
				applied = true;
				// Pick own transform from the first child body.
				SE3 se3_in_physics = body->get_se3_in_physics_raw();
				set_se3_raw( se3_in_physics );
				inv_se3_in_physics = se3_in_physics.inverse();
			}
			// For each body compute transform relative to assembly.
			SE3 body_se3 = body->get_se3_in_physics_raw();
			body_se3 = inv_se3_in_physics * body_se3;
			body->set_se3_raw( body_se3 );
		}
	}
	RefFrameNode::ign_process( delta );
}

void RefFrameAssemblyNode::on_parent_jumped()
{
	RefFrameNode::on_parent_jumped();

	// Let all children know recursively.
	const int qty = get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * ch = get_child( i );
		RefFrameNode * rf = Object::cast_to<RefFrameNode>( ch );
		if ( rf != nullptr )
		{
			rf->on_parent_jumped();
		}
	}
}







}



