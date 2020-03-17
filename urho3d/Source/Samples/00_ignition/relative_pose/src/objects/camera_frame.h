
#ifndef __CAMERA_FRAME_H_
#define __CAMERA_FRAME_H_

#include "ref_frame.h"

namespace Ign
{

class CameraFrame: public RefFrame
{
    URHO3D_OBJECT( CameraFrame, RefFrame )
public:
    static void RegisterComponent( Context * context );

    CameraFrame( Context * context );
    ~CameraFrame();

    void ApplyControls( const Controls & ctrl, Float dt ) override;

    void setUseSurfFrame( bool en );
    RefFrame * CameraOrigin();

protected:
    void OnSceneSet( Scene * scene ) override;

    void adjustSurfQuat();
    static RefFrame * orbitingFrame( RefFrame * rf );
public:
    void refStateChanged() override;
    void assignCameraNode();
    /// Camera node.
    SharedPtr<Node> node_;

    /// Camera parameters.
    Float yaw_, pitch_;
    Float dist_;
    bool useSurfFrame_;
    Quaterniond surfQ_;
    static const Float alpha_;
};


}


#endif

