
#include "ref_frame_root.h"
#include "core/config/engine.h"

namespace Ign
{

void RefFrameRoot::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_time_dilation", "en" ), &RefFrameRoot::set_time_dilation );
	ClassDB::bind_method( D_METHOD( "get_time_dilation" ),       &RefFrameRoot::get_time_dilation );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT, "time_dilation" ), "set_time_dilation", "get_time_dilation" );

}

RefFrameRoot::RefFrameRoot()
	: RefFrameNode()
{
	time_dilation = 1.0;

	set_process( true );
	set_physics_process( true );
}

RefFrameRoot::~RefFrameRoot()
{
}

void RefFrameRoot::set_time_dilation( real_t gain )
{
	time_dilation = gain;
}

real_t RefFrameRoot::get_time_dilation() const
{
	return time_dilation;
}

void RefFrameRoot::_notification( int p_notification )
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

void RefFrameRoot::pre_process_children( RefFrameNode * ref_frame, real_t delta )
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

void RefFrameRoot::process_children( RefFrameNode * ref_frame, real_t delta )
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

void RefFrameRoot::post_process_children( RefFrameNode * ref_frame, real_t delta )
{
	ref_frame->ign_post_process( delta );

	int qty = ref_frame->get_child_count();
	for ( int i=0; i<qty; i++ )
	{
		Node * n = ref_frame->get_child( i );
		RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
		if ( ch_ref_frame != nullptr )
			post_process_children( ch_ref_frame, delta );
	}
}

void RefFrameRoot::physics_pre_process_children( RefFrameNode * ref_frame, real_t delta )
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

void RefFrameRoot::physics_process_children( RefFrameNode * ref_frame, real_t delta )
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

void RefFrameRoot::physics_post_process_children( RefFrameNode * ref_frame, real_t delta )
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

//void RefFrameRoot::_ign_pre_process( real_t delta )
//{
//}
//
//void RefFrameRoot::_ign_process( real_t delta )
//{
//}
//
//void RefFrameRoot::_ign_post_process( real_t delta )
//{
//}






}





