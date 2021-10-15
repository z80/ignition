
#include "camera_orb_2.h"

using namespace Urho3D;

namespace Osp
{

CameraOrb2::CameraOrb2( Context * c )
    : LogicComponent( c ),
      origin( Vector3::ZERO )
{
    SubscribeToEvents();

    az = 0.0f;
    el = 30.0 / 180.0 * 3.14;
    r = -3.0;
    va = 1.3;
    vr = 1.1;

    // Came camera position match the numbers.
    //updateCamera();
}

CameraOrb2::~CameraOrb2()
{

}

void CameraOrb2::SubscribeToEvents()
{
    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( CameraOrb2, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( CameraOrb2, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEMOVE,       URHO3D_HANDLER( CameraOrb2, HandleMouseMove ) );
    SubscribeToEvent( E_MOUSEWHEEL,      URHO3D_HANDLER( CameraOrb2, HandleMouseWheel ) );

}

void CameraOrb2::DelayedStart()
{
    updateCamera();
}

void CameraOrb2::HandleMouseDown( StringHash t, VariantMap & e )
{
    const int b = e[MouseButtonDown::P_BUTTON].GetInt();
    if ( b != SDL_BUTTON_MIDDLE )
        return;
    activated = true;
}

void CameraOrb2::HandleMouseUp( StringHash t, VariantMap & e )
{
    const int b = e[MouseButtonDown::P_BUTTON].GetInt();
    if ( b != SDL_BUTTON_MIDDLE )
        return;

    activated = false;
}

void CameraOrb2::HandleMouseMove( StringHash t, VariantMap & e )
{
    if ( !activated )
        return;

    Graphics * graphics = GetSubsystem<Graphics>();
    const int w = graphics->GetWidth();
    const int h = graphics->GetHeight();
    const int dx = e[MouseMove::P_DX].GetInt();
    const int dy = e[MouseMove::P_DY].GetInt();

    // Yes, in both cases use height.
    const float d_az = va * (float)dx / (float)(h);
    const float d_el = va * (float)dy / (float)(h);

    az += d_az;
    el += d_el;

    updateCamera();
}

void CameraOrb2::HandleMouseWheel( StringHash t, VariantMap & e )
{
    //if ( !activated )
    //    return;

    const int w = e[MouseWheel::P_WHEEL].GetInt();

    if ( w > 0 )
        r = r / vr;
    else
        r = r * vr;

    updateCamera();
}

void CameraOrb2::updateCamera()
{
    Camera * cam = GetNode()->GetComponent<Camera>();
    if ( !cam )
        return;

    const float az2 = az/2.0f;
    const float el2 = el/2.0f;

    const Quaternion qy( std::cos(az2), 0.0f, std::sin(az2), 0.0f );
    const Quaternion qx( std::cos(el2), std::sin(el2), 0.0f, 0.0f );
    const Quaternion q = qy * qx;

    Vector3 vr( 0.0, 0.0, r );
    vr = q * vr + origin;

    Node * n = cam->GetNode();
    n->SetRotation( q );
    n->SetPosition( vr );
}

void CameraOrb2::setOrigin( const Vector3 & at )
{
    origin = at;
}



}



