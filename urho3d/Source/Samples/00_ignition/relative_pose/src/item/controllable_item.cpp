
#include "controllable_item.h"

namespace Ign
{

void ControllableItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<ControllableItem>();

    URHO3D_ATTRIBUTE( "CreatedByUserId", int, createdByUserId_, -1, AM_DEFAULT );
}

ControllableItem::ControllableItem( Context * context )
    : Component( context )
{
    createdByUserId_ = -1;
}

ControllableItem::~ControllableItem()
{

}

void ControllableItem::ApplyControls( const Controls & ctrl )
{

}

bool ControllableItem::AcceptsControls() const
{
    return false;
}

void ControllableItem::SetCreatedBy( int userId )
{
    createdByUserId_ = userId;
}

int ControllableItem::CreatedBy() const
{
    return createdByUserId_;
}

bool ControllableItem::CanBeControlledBy( int userId ) const
{
    const bool res = ( ( userId ==  createdByUserId_) && ( userId >= 0 ) );
    return res;
}



}


