
#include "controllable_item.h"
#include "environment.h"

namespace Ign
{

void ControllableItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<ControllableItem>();

    URHO3D_ATTRIBUTE(    "CreatedByUserId",       int,    createdByUserId_,    -1,     AM_DEFAULT );
    // name, typeName, variable, postSetCallback, defaultValue, mode
    URHO3D_ATTRIBUTE_EX( "SelectedIndsString", String, selectedIndsString_, SelectedStringUpdated, String(""), AM_DEFAULT );
}

ControllableItem::ControllableItem( Context * context )
    : Component( context )
{
    createdByUserId_  = -1;
}

ControllableItem::~ControllableItem()
{

}

void ControllableItem::ApplyControls( const Controls & ctrl )
{

}

bool ControllableItem::AcceptsControls( int userId ) const
{
    return true;
}

bool ControllableItem::IsSelectable() const
{
    return false;
}

void ControllableItem::Trigger( const VariantMap & data )
{

}

void ControllableItem::SetCreatedBy( int userId )
{
    createdByUserId_ = userId;

    MarkNetworkUpdate();
}

int ControllableItem::CreatedBy() const
{
    return createdByUserId_;
}

bool ControllableItem::Select( int userId )
{
    const bool selectable = IsSelectable();
    if ( !selectable )
        return false;

    Unselect( userId );
    selectedByUserIds_.Push( userId );

    UpdateSelectedString();

    return true;
}

void ControllableItem::Unselect( int userId )
{
    selectedByUserIds_.RemoveSwap( userId );

    UpdateSelectedString();
}

const Vector<int> & ControllableItem::SelectedBy() const
{
    return selectedByUserIds_;
}

const bool ControllableItem::SelectedBy( int userId ) const
{
    Vector<int>::ConstIterator it = selectedByUserIds_.Find( userId );
    const bool ok = ( it != selectedByUserIds_.End() );
    return ok;
}

void ControllableItem::SelectedStringUpdated()
{
    selectedIndStrings_ = selectedIndsString_.Split( ',' );
    const unsigned qty = selectedIndStrings_.Size();
    selectedByUserIds_.Clear();
    selectedByUserIds_.Reserve( qty );
    for ( unsigned i=0; i<qty; i++ )
    {
        const String & stri = selectedIndStrings_[i];
        const int id = ToInt( stri );
        selectedByUserIds_.Push( id );
    }
}

Environment * ControllableItem::env()
{
    Scene * s = GetScene();
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        if ( !c )
            continue;
        Environment * e = c->Cast<Environment>();
        if ( e )
            return e;
    }

    return nullptr;
}

void ControllableItem::UpdateSelectedString()
{
    const unsigned qty = selectedByUserIds_.Size();
    selectedIndStrings_.Clear();
    if ( qty > 0 )
    {
        const unsigned lastInd = qty - 1;
        for ( unsigned i=0; i<qty; i++ )
        {
            const int id = selectedByUserIds_[i];
            const String stri( id );
            selectedIndStrings_ += stri;
            if ( i != lastInd )
                selectedIndStrings_ += ",";
        }
    }

    MarkNetworkUpdate();
}




}


