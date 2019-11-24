
#include "rep_comp.h"

namespace Ign
{

void RepComp::RegisterObject( Context * context )
{
    context->RegisterFactory<RepComp>();

    URHO3D_ACCESSOR_ATTRIBUTE( "Name", Name, SetName, String, "Network Name", AM_DEFAULT );
    URHO3D_ATTRIBUTE( "px", Float, v_.x_, 0.0,  AM_DEFAULT );
    URHO3D_ATTRIBUTE( "py", Float, v_.y_, 1.0,  AM_DEFAULT );
    URHO3D_ATTRIBUTE( "pz", Float, v_.z_, 2.0,  AM_DEFAULT );
}

RepComp::RepComp( Context * context )
    : Component( context )
{
    name_ = "default name";

    URHO3D_LOGINFO( "RepComp created" );
}

RepComp::~RepComp()
{
    URHO3D_LOGINFO( "RepComp destroyed" );
}

void RepComp::SetName( const String & name )
{
    name_ = name;

    MarkNetworkUpdate();
    //SetAttribute( "Name", name );

    URHO3D_LOGINFOF( "Name is set to %s", name.CString() );
}

const String & RepComp::Name() const
{
    return name_;
}

void RepComp::SetPos( const Vector3d & p )
{
    v_ = p;

    MarkNetworkUpdate();

    URHO3D_LOGINFOF( "Pos is set to %f, %f, %f", v_.x_, v_.y_, v_.z_ );
}

const Vector3d & RepComp::Pos() const
{
    return v_;
}




}

