
#include "celestial_rotation_ref.h"
#include "ref_frame_node.h"

namespace Ign
{

void CelestialRotationRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("init", "up", "period_hrs"), &CelestialRotationRef::init );
	ClassDB::bind_method( D_METHOD("process", "dt", "rf"),      &CelestialRotationRef::process );
}

CelestialRotationRef::CelestialRotationRef()
	: Reference()
{
}

CelestialRotationRef::~CelestialRotationRef()
{
}

void CelestialRotationRef::init( const Vector3 & up, real_t period_hrs )
{
	celestial_rotation.init( Vector3d( up.x, up.y, up.z ), period_hrs );
}

void CelestialRotationRef::process( real_t dt, Node * rf )
{
	RefFrameNode * rf_node = Node::cast_to<RefFrameNode>( rf );
	if ( rf_node )
		rf_node->se3_ = celestial_rotation.process( dt );
}




}




