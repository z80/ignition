
#ifndef __CONTROLLABLE_ITEM_H_
#define __CONTROLLABLE_ITEM_H_

#include "Urho3D/Urho3DAll.h"

using namespace Urho3D;

namespace Ign
{

class ControllableItem: public Urho3D::Component
{
    URHO3D_OBJECT( ControllableItem, Component )
public:
    static void RegisterComponent( Context * context );

    ControllableItem( Context * context );
    virtual ~ControllableItem();

    virtual void ApplyControls( const Controls & ctrl );
    virtual bool AcceptsControls( int userId ) const;
    virtual bool IsSelectable() const;

    void SetCreatedBy( int userId );
    int  CreatedBy() const;

    virtual bool Select( int userId );
    void Unselect( int userId );
    const Vector<int> & SelectedBy() const;

    // Need to add more sophisticated functionality to this later...

    void SelectedStringUpdated();
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




