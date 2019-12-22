
#include "camera_frame.h"
#include "environment.h"

namespace Ign
{

void CameraFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<CameraFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );

    URHO3D_ATTRIBUTE( "UserId", int, userId_, -1, AM_DEFAULT );

}

CameraFrame::CameraFrame( Context * context )
    : RefFrame( context )
{

}

CameraFrame::~CameraFrame()
{

}

void CameraFrame::assignCameraNode()
{
    Scene * s = GetScene();
    if ( !s )
      return;

}

void CameraFrame::OnSceneSet( Scene * scene )
{

}


}


