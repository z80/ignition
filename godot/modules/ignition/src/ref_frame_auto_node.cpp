
#include "ref_frame_auto_node.h"

namespace Ign
{

void RefFrameAutoNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD( "set_distance_scaler", "scaler"), &RefFrameAutoNode::set_distance_scaler );
	ClassDB::bind_method( D_METHOD( "get_distance_scaler"), &RefFrameAutoNode::get_distance_scaler, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD( "set_root_path", "path"), &RefFrameAutoNode::set_root_path );
	ClassDB::bind_method( D_METHOD( "get_root_path"), &RefFrameAutoNode::get_root_path, Variant::NODE_PATH );

	ClassDB::bind_method( D_METHOD( "distance_scaled_t"), &RefFrameAutoNode::distance_scaled_t, Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD( "distance_scale" ), &RefFrameAutoNode::distance_scale, Variant::REAL );

}

void RefFrameAutoNode::_notification( int p_what )
{
	switch ( p_what )
	{
	case NOTIFICATION_PROCESS:
		process_transform();
		break;
	default:
		break;
	}
}

RefFrameAutoNode::RefFrameAutoNode()
	: RefFrameNode()
{
	apply_scale = true;
	scale       = 1.0;
	// Make call "_notification( p_what )" when
	// "_process( delta )" is to be called.
	set_process( true );
}

RefFrameAutoNode::~RefFrameAutoNode()
{
}

void RefFrameAutoNode::set_distance_scaler( Ref<DistanceScalerRef> new_scaler )
{
	scaler = new_scaler;
}

Ref<DistanceScalerRef> RefFrameAutoNode::get_distance_scaler() const
{
	return scaler;
}

void RefFrameAutoNode::set_root_path( const NodePath & root )
{
	root_path = root;
}

const NodePath & RefFrameAutoNode::get_root_path() const
{
	return root_path;
}

Transform RefFrameAutoNode::distance_scaled_t() const
{
	Transform t = se3_root_.transform();
	t.basis.scale( Vector3( scale, scale, scale ) );
	t.origin = Vector3( scaled_r.x_, scaled_r.y_, scaled_r.z_ );
	return t;
}

real_t RefFrameAutoNode::distance_scale() const
{
	return scale;
}

void RefFrameAutoNode::set_apply_scale( bool en )
{
	apply_scale = en;
}

bool RefFrameAutoNode::get_apply_scale() const
{
	return apply_scale;
}

void RefFrameAutoNode::process_transform()
{
	Node * n = get_node_or_null( root_path );
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( n );
	se3_root_ = relative_( rf );
	if ( ( !apply_scale ) || ( scaler.ptr() == nullptr ) )
		scaled_r = se3_root_.r_;
	else
	{
		const Vector3d r = se3_root_.r_;
		const Float dist = r.Length();
		scale = scaler->scale( dist );
		scaled_r = r * scale;
	}

	// Apply transform
	Transform t = se3_root_.transform();
	t.basis.scale( Vector3( scale, scale, scale ) );
	t.origin = Vector3( scaled_r.x_, scaled_r.y_, scaled_r.z_ );
	set_transform( t );
}


}

