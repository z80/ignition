
#ifndef __CONTROLLABLE_ITEM_H_
#define __CONTROLLABLE_ITEM_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

class Environment;

class ControllableItem: public Urho3D::Component
{
    URHO3D_OBJECT( ControllableItem, Component )
public:
    static void RegisterComponent( Context * context );

    ControllableItem( Context * context );
    virtual ~ControllableItem();

    virtual void ApplyControls( const Controls & ctrl, Float dt );
    virtual bool AcceptsControls( int userId ) const;
    virtual bool IsSelectable() const;

    // Reaction on trigger (user who currently has this item selected hits space).
    virtual void Trigger( const VariantMap & data );

    void SetCreatedBy( int userId );
    int  CreatedBy() const;

    virtual bool Select( int userId );
    void Unselect( int userId );
    const Vector<int> & SelectedBy() const;
    const bool SelectedBy( int userId ) const;

    // Need to add more sophisticated functionality to this later...

    void SelectedStringUpdated();

    Environment * env();
protected:
    void UpdateSelectedString();
    int         createdByUserId_;
    Vector<int> selectedByUserIds_;
    String      selectedIndsString_;
    Vector<String> selectedIndStrings_;

    //int parentControllableItem_;
    //bool enabled_;
};


}



#endif




