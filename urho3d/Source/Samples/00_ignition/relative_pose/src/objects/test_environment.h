
#ifndef __TEST_ENVIRONMENT_H_
#define __TEST_ENVIRONMENT_H_

#include "environment.h"

namespace Ign
{

class TestEnvironment: public Environment
{
    URHO3D_OBJECT( TestEnvironment, Environment )
public:
    static void RegisterComponent( Context * context );

    TestEnvironment( Context * context );
    ~TestEnvironment();

    void DelayedStart() override;
    bool ClientConnected( int id, const VariantMap & identity, String & errMsg ) override;

protected:
    void CreateReplicatedContentServer() override;
    void CreateReplicatedContentClient( CameraFrame * camera ) override;

    void contentServerTestSystem();
    void contentClientCameraOnly( CameraFrame * camera );

    void contentServerPlanet();
    void contentClientCharacterCube( CameraFrame * camera );
};

}




#endif



