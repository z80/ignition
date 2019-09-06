
#ifndef __CAMERA_ORB_2_H_
#define __CAMERA_ORB_2_H_


#include <Urho3D/Urho3DAll.h>

namespace Osp
{

using namespace Urho3D;

/**
 * @brief The Entity class
 * This is the base class for all interacting objects.
 */
class CameraOrb2: public LogicComponent
{
    URHO3D_OBJECT( CameraOrb2, LogicComponent )
public:
    CameraOrb2( Context * c );
    ~CameraOrb2();

    void SubscribeToEvents();

    void DelayedStart();

    void HandleMouseDown( StringHash t, VariantMap & e );
    void HandleMouseUp( StringHash t, VariantMap & e );
    void HandleMouseMove( StringHash t, VariantMap & e );
    void HandleMouseWheel( StringHash t, VariantMap & e );

    void updateCamera();

    void setOrigin( const Vector3 & at );
private:
    float az, el, r;
    float va, vr;
    bool  activated;
    Vector3 origin;
};

}


#endif



