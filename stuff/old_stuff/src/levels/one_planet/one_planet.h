
#ifndef __ONE_PLANET_H_
#define __ONE_PLANET_H_

#include "BaseLevel.h"
#include "tech_tree.h"
#include "block.h"
#include "design_manager.h"
#include "planet_base.h"
#include "player.h"

namespace Urho3D
{
    class PhysicsWorld2;
}

namespace Osp
{

class OnePlanet: public BaseLevel
{
URHO3D_OBJECT( OnePlanet, BaseLevel )

public:
    /// Construct.
    OnePlanet( Context * context );

    ~OnePlanet();

protected:
    void Init() override;
    void Finit() override;

private:
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    SharedPtr<UIElement> panel;
    SharedPtr<Player>    player;
    int mouseX,     mouseY,
        mousePrevX, mousePrevY;
public:
    bool select();

    void HandleUpdate( StringHash t, VariantMap & e );
    void HandlePostRenderUpdate( StringHash t, VariantMap & e );
    // Subscribe to fixed timestep physics updates for setting or applying controls
    void HandlePhysicsPreStep( StringHash t, VariantMap & e );
    // Subscribe HandlePostUpdate() method for processing update events. Subscribe to PostUpdate instead
    // of the usual Update so that physics simulation has already proceeded for the frame, and can
    // accurately follow the object with the camera
    void HandlePostUpdate( StringHash t, VariantMap & e );

    // Keyboard and mouse input for editor.
    void HandleMouseDown( StringHash t, VariantMap & e );
    void HandleMouseUp( StringHash t, VariantMap & e );
    void HandleMouseMove( StringHash t, VariantMap & e );
    void HandleKeyDown( StringHash t, VariantMap & e );
    void HandleKeyUp( StringHash t, VariantMap & e );

    // Change to workshop.
    void HandleToWorkshop( StringHash t, VariantMap & e );
};

}


#endif


