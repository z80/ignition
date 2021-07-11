
#include "save_load.h"


namespace Ign
{

void serialize_vector( const Vector3d & v, const String & name, Dictionary & data )
{
    String n = name + String( "_x" );
    data[n] = v.x_;
    n = name + String( "_y" );
    data[n] = v.y_;
    n = name + String( "_z" );
    data[n] = v.z_;
}

Vector3d deserialize_vector( const String & name, const Dictionary & data )
{
    Vector3d v;
    String n = name + String( "_x" );
    v.x_ = data[n];
    n = name + String( "_y" );
    v.y_ = data[n];
    n = name + String( "_z" );
    v.z_ = data[n];

    return v;
}


void serialize_matrix( const Matrix3d & v, const String & name, Dictionary & data )
{
    String n = name + String( "_00" );
    data[n] = v.m00_;
    n = name + String( "_01" );
    data[n] = v.m01_;
    n = name + String( "_02" );
    data[n] = v.m01_;

    n = name + String( "_10" );
    data[n] = v.m10_;
    n = name + String( "_11" );
    data[n] = v.m11_;
    n = name + String( "_12" );
    data[n] = v.m11_;

    n = name + String( "_20" );
    data[n] = v.m20_;
    n = name + String( "_21" );
    data[n] = v.m21_;
    n = name + String( "_22" );
    data[n] = v.m21_;
}


Matrix3d deserialize_matrix( const String & name, const Dictionary & data )
{
    Matrix3d v;
    String n = name + String( "_00" );
    v.m00_ = data[n];
    n = name + String( "_01" );
    v.m01_ = data[n];
    n = name + String( "_01" );
    v.m02_ = data[n];

    n = name + String( "_10" );
    v.m10_ = data[n];
    n = name + String( "_11" );
    v.m11_ = data[n];
    n = name + String( "_11" );
    v.m12_ = data[n];

    n = name + String( "_20" );
    v.m20_ = data[n];
    n = name + String( "_21" );
    v.m21_ = data[n];
    n = name + String( "_21" );
    v.m22_ = data[n];

	return v;
}

}




