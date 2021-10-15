
#ifndef __PLANET_BASE_H_
#define __PLANET_BASE_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "block.h"
#include "assembly.h"
#include "data_types.h"
#include "kepler_mover.h"
#include "kepler_rotator.h"
#include "planet_forces.h"

namespace Osp
{

using namespace Urho3D;
class WorldMover;

class PlanetBase: public ItemBase
{
    URHO3D_OBJECT( PlanetBase, ItemBase )
public:
    PlanetBase( Context * ctx );
    virtual ~PlanetBase();

    bool canOrbit( const ItemBase * a ) const;
    virtual Float GM() const;

    virtual Vector3d relR() const override;
    virtual Quaterniond relQ() const override;
    virtual Vector3d relV() const override;
    virtual Vector3d relW() const override;


    virtual void Start() override;
    virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest) override;

    /// When player is on the surface the dynamics is
    /// simulated for objects close to the center and close enough
    /// Objects in atmosphere but far away are not simulated.
    virtual void updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist );
    /// Initialize collision objects when player enteres this area.
    virtual void initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist );
    /// Finalize collisions as player leaves the area.
    virtual void finitCollisions( PhysicsWorld2 * w2 );

    virtual bool load( const JSONValue & v );

public:
    // Need to implement these and call collision objects updates here.
    void subscribeToWorldEvents();
    void OnWorldPlanetChanged( StringHash eventType, VariantMap & eventData );
    void OnWorldAdjusted( StringHash eventType, VariantMap & eventData );

    // Planet name for making hierarchy in config file.
    String name;
    // There are at least 3 nodes.
    // 1-st translating, moving the orbit.
    // 2-d  attached to the 1-st one but also rotating. Physcis world and collision bodies
    // should be attached to this one. And atmosphere effects should also be taken into
    // account here.
    // 3-d, 4-th, etc. for graphics.
    // Need only a pointer to the 2-d one here as 1-st one can be obtained by "GetNode()".
    SharedPtr<Node>  dynamicsNode;

    // Thing which defines forces.
    SharedPtr<PlanetForces>  forces;
    // Planet mover and rotator.
    // Mover updates the very root node.
    SharedPtr<KeplerMover>   mover;
    // Rotator updates only "dynamicsNode".
    SharedPtr<KeplerRotator> rotator;

    // Pointers to dynamics world.
    // Just to sabe time on recursively traversing nodes tree.
    SharedPtr<Component> physicsWorld;
    SharedPtr<Component> worldMover;
};

}



#endif

