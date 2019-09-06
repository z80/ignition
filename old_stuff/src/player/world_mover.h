
#ifndef __WORLD_MOVER_H_
#define __WORLD_MOVER_H_

#include "kepler_mover.h"
#include "planet_base.h"
#include "assembly.h"

namespace Osp
{

class Assembly;
class PlanetBase;

class WorldMover: public KeplerMover
{
    URHO3D_OBJECT( WorldMover, KeplerMover )
public:
    WorldMover( Context * ctx );
    ~WorldMover();

    void Start() override;
    void Update( float dt ) override;
    //void PostUpdate( float dt ) override;

    void setR( const Vector3d & new_r ) override;
    void setV( const Vector3d & new_v ) override;

    void DrawDebugGeometry( DebugRenderer * debug, bool depthTest ) override;

protected:
    void subscribeToEvents();
    // Whenever it is moved a message is sent to everyone.
    // Whenever player selects an assembly a message is sent as well
    // to let this mover know where to displace the world.
    void OnAssemblySelected( StringHash eventType, VariantMap & eventData );

    // Switches to orbit/ground.
    bool needOrbit() const;
    bool needGround() const;
    void switchToOrbiting();
    void switchToGrounding();

    // While orbiting check for planet of influence.
    // This should be exactly the same as for assembly.
    void checkInfluence();
    PlanetBase * planetOfInfluence();


    void switchTo( Assembly * assembly );
    void adjustToTarget();
    void adjustEvent( const Vector3d & dr, const Vector3d & dv, bool orbiting );

public:
    SharedPtr<PlanetBase> planet;
    SharedPtr<Assembly>   assembly;
    // Don't need this. As "active" attribute describes that.
    //bool                  inAtmosphere;
};

}





#endif


