
#include "ref_frame_root_node.h"
#include "core/config/engine.h"

namespace Ign
{

void RefFrameRootNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_time_dilation", "en" ), &RefFrameRootNode::set_time_dilation );
	ClassDB::bind_method( D_METHOD( "get_time_dilation" ),       &RefFrameRootNode::get_time_dilation );

	ClassDB::bind_method( D_METHOD( "set_camera_node", "camera_node" ), &RefFrameRootNode::set_camera_node );
	ClassDB::bind_method( D_METHOD( "get_camera_node" ),                &RefFrameRootNode::get_camera_node );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "time_dilation" ), "set_time_dilation", "get_time_dilation" );

}

RefFrameRootNode::RefFrameRootNode()
	: RefFrameNode()
{
	time_dilation = 1.0;
	camera_node_ = nullptr;

	set_process( true );
	set_physics_process( true );
}

RefFrameRootNode::~RefFrameRootNode()
{
}

void RefFrameRootNode::set_time_dilation( real_t gain )
{
	time_dilation = gain;
}

real_t RefFrameRootNode::get_time_dilation() const
{
	return time_dilation;
}

void RefFrameRootNode::set_camera_node( Node * node )
{
	camera_node_ = Object::cast_to<RefFrameNode>( node );
}

Node * RefFrameRootNode::get_camera_node() const
{
	return camera_node_;
}

void RefFrameRootNode::_notification( int p_notification )
{
	const bool is_editor = Engine::get_singleton()->is_editor_hint();
	if ( is_editor )
		return;

	switch (p_notification)
	{
		case NOTIFICATION_PROCESS:
		{
			const real_t delta = get_process_delta_time();
			const real_t total_delta = delta * time_dilation;
			pre_process_children( this, total_delta );
			process_children( this, total_delta );
			post_process_children( this, total_delta );
			break;
		}

		case NOTIFICATION_PHYSICS_PROCESS:
		{
			const real_t delta = get_physics_process_delta_time();
			const real_t total_delta = delta * time_dilation;
			physics_pre_process_children( this, total_delta );
			physics_process_children( this, total_delta );
			physics_post_process_children( this, total_delta );
			break;
		}
	}
}

void RefFrameRootNode::pre_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_pre_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			pre_process_children( ch_ref_frame, delta );
	}
}

void RefFrameRootNode::process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			process_children( ch_ref_frame, delta );
	}
}

void RefFrameRootNode::post_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_post_process( delta );

	// _relative_to_camera() is called the last, after even "ign_post_process()".
	if ( camera_node_ != nullptr )
	{
		const bool needs_relative = ref_frame->get_needs_relative_to_camera();
		if ( needs_relative )
		{
			const SE3 se3 = ref_frame->relative_( camera_node_ );
			ref_frame->on_relative_to_camera( this, camera_node_, se3 );
		}
	}

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			post_process_children( ch_ref_frame, delta );
	}
}

void RefFrameRootNode::physics_pre_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_physics_pre_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			physics_pre_process_children( ch_ref_frame, delta );
	}
}

void RefFrameRootNode::physics_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_physics_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			physics_process_children( ch_ref_frame, delta );
	}
}

void RefFrameRootNode::physics_post_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_physics_post_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			physics_post_process_children( ch_ref_frame, delta );
	}
}

//void RefFrameRootNode::_ign_pre_process( real_t delta )
//{
//}
//
//void RefFrameRootNode::_ign_process( real_t delta )
//{
//}
//
//void RefFrameRootNode::_ign_post_process( real_t delta )
//{
//}






}





