
#ifndef __CHARACTER_CUBE_H_
#define __CHARACTER_CUBE_H_

#include "physics_character_item.h"

using namespace Urho3D;

namespace Ign
{

class CharacterCube: public PhysicsCharacterItem
{
    URHO3D_OBJECT( CharacterCube, PhysicsCharacterItem )
public:
    static void RegisterComponent( Context * context );

    CharacterCube( Context * context );
    virtual ~CharacterCube();

    void ApplyControls( const Controls & ctrl ) override;
    bool AcceptsControls( int userId ) const override;
    bool IsSelectable() const override;

    // Reaction on trigger (user who currently has this item selected hits space).
    void Trigger( const VariantMap & data ) override;

protected:
    virtual void createVisualContent( Node * n ) override;
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;
};

}


#endif


