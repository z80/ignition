
#include "celestial_rotation_ref.h"
#include "ref_frame_node.h"
#include "save_load.h"

namespace Ign
{

void CelestialRotationRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("init", "up", "period_hrs"), &CelestialRotationRef::init );
    ClassDB::bind_method( D_METHOD("process_rf", "dt", "rf"),   &CelestialRotationRef::process_rf );
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

void CelestialRotationRef::process_rf( real_t dt, Node * rf )
{
    RefFrameNode * rf_node = Node::cast_to<RefFrameNode>( rf );
    if ( rf_node )
        rf_node->se3_ = celestial_rotation.process( dt );
}

Dictionary CelestialRotationRef::serialize() const
{
    Dictionary data;
    data["spinning"] = celestial_rotation.spinning;
    data["period"]   = celestial_rotation.period;
    data["time"]     = celestial_rotation.time;
    serialize_quat( celestial_rotation.axis_orientation, "axis_orientation", data );
    data["se3"] = celestial_rotation.se3.serialize();

	return data;
}

bool CelestialRotationRef::deserialize( const Dictionary & data )
{
	celestial_rotation.spinning = data["spinning"];
	celestial_rotation.period = data["period"];
	celestial_rotation.time = data["time"];
	celestial_rotation.axis_orientation = deserialize_quat( "axis_orientation", data );
	celestial_rotation.se3.deserialize( data["se3"] );

    return true;
}







}




