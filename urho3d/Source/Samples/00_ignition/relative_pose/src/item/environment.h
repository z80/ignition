
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

class Environment: public LogicComponent
{
    URHO3D_OBJECT( Environment, LogicComponent )
public:
    Environment( Context * context );
    ~Environment();

    static void RedisterComponent( Context * context );

    bool IsClient() const;
    bool IsServer() const;

    void Start() override;
    void DelayedStart() override;
    void Stop() override;
    void Update( float timeStep ) override;

private:
    void IncrementTime( float secs_dt );
    void UpdateDynamicNodes( float secs );
    void UpdateEvolvingNodes( Timestamp ticks );

    /// Global time
    Timestamp T_;
    /// Delta times in seconds and in ticks.
    Float     secsDt_;
    Timestamp ticksDt_;
};

}


#endif


