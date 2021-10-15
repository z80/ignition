
#ifndef __KEPLER_TIME_COUNTER_H_
#define __KEPLER_TIME_COUNTER_H_

#include <Urho3D/Urho3DAll.h>
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

class ItemNode;

class KeplerTimeCounter: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( KeplerTimeCounter, LogicComponent )
public:
    KeplerTimeCounter( Context * ctx );
    ~KeplerTimeCounter();

    /// Register object factory.
    static void RegisterObject( Context * context);


    void Start() override;
    void Update( float dt_ ) override;
    /// Update Kepler nodes in accordance with what current time is.
    void UpdateNodes();
    /// Set time lapse speed gain or attenuation.
    void SetTimeAcceleration( int xN );

    /// Specify physics world node.
    void SetPhysicsWorldItem( ItemNode * node );


    Timestamp dt_;
    Timestamp time_;
    int       xN_;
    static const Timestamp ONE_SECOND;
    static const Float    _ONE_SECOND;

private:
    class PD;
    PD * pd;
};

}

#endif


