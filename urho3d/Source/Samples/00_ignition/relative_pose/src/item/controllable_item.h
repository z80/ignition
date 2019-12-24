
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
    virtual bool AcceptsControls() const;
    void SetCreatedBy( int userId );
    int  CreatedBy() const;
    bool CanBeControlledBy( int userId ) const;

    // Need to add more sophisticated functionality to this later...

protected:
    int createdByUserId_;

    //int parentControllableItem_;
    //bool enabled_;
};


}



#endif




