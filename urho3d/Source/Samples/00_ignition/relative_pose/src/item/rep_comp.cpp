
#include "rep_comp.h"

namespace Ign
{

void RepComp::RegisterObject( Context * context )
{
    context->RegisterFactory<RepComp>();

    URHO3D_ACCESSOR_ATTRIBUTE( "Name", Name, SetName, String, "Network Name", AM_DEFAULT );
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



}

