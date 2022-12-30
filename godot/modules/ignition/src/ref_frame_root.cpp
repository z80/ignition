
#include "ref_frame_root.h"

namespace Ign
{

void RefFrameRoot::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_time_dilation", "en" ), &RefFrameRoot::set_time_dilation );
	ClassDB::bind_method( D_METHOD( "get_time_dilation" ),       &RefFrameRoot::get_time_dilation, Variant::REAL );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "time_dilation" ), "set_time_dilation", "get_time_dilation" );

}

RefFrameRoot::RefFrameRoot()
	: RefFrameNode()
{
	time_dilation = 1.0;

	set_process( true );
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
	switch (p_notification)
	{
		case NOTIFICATION_PROCESS:
		{
			const real_t delta = get_process_delta_time();
			const real_t total_delta = delta * time_dilation;
			_process_children( this, total_delta );
			break;
		}

		case NOTIFICATION_PHYSICS_PROCESS:
		{
			const real_t delta = get_physics_process_delta_time();
			const real_t total_delta = delta * time_dilation;
			_physics_process_children( this, total_delta );
			break;
		}
	}
}


void RefFrameRoot::_process_children( RefFrameNode * ref_frame, real_t delta )
{
	if ( ref_frame == nullptr )
		return;

	{
		ref_frame->_ign_pre_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_process_children( ch_ref_frame, delta );
		}
	}

	{
		ref_frame->_ign_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_process_children( ch_ref_frame, delta );
		}
	}

	{
		ref_frame->_ign_post_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_process_children( ch_ref_frame, delta );
		}
	}
}

void RefFrameRoot::_physics_process_children( RefFrameNode * ref_frame, real_t delta )
{
	if ( ref_frame == nullptr )
		return;

	{
		ref_frame->_ign_physics_pre_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_physics_process_children( ch_ref_frame, delta );
		}
	}

	{
		ref_frame->_ign_physics_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_physics_process_children( ch_ref_frame, delta );
		}
	}

	{
		ref_frame->_ign_physics_post_process( delta );

		int qty = ref_frame->get_child_count();
		for ( int i=0; i<qty; i++ )
		{
			Node * n = ref_frame->get_child( i );
			RefFrameNode * ch_ref_frame = Object::cast_to<RefFrameNode>( n );
			_physics_process_children( ch_ref_frame, delta );
		}
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





