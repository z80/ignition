
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

    void ApplyControls( const Controls & ctrl ) override;

    RefFrame * CameraOrigin();

protected:
    void OnSceneSet( Scene * scene ) override;

public:
    void refStateChanged() override;
    void assignCameraNode();
    /// Camera node.
    SharedPtr<Node> node_;
};


}


#endif

