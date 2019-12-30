
#include "ref_frame_register.h"

#include "ref_frame.h"
#include "physics_frame.h"
#include "physics_item.h"
#include "evolving_frame.h"
#include "rotating_frame.h"
#include "orbiting_frame.h"
#include "camera_frame.h"
#include "environment.h"

#include "main_menu.h"

#include "rep_comp.h"
#include "static_mesh.h"
#include "infinite_plane.h"
#include "ico_planet.h"
#include "test_environment.h"

namespace Ign
{

void RegisterComponents( Context * context )
{
    RefFrame::RegisterComponent( context );
    PhysicsFrame::RegisterComponent( context );
    PhysicsItem::RegisterComponent( context );
    PhysicsItem::RegisterComponent( context );
    EvolvingFrame::RegisterComponent( context );
    RotatingFrame::RegisterComponent( context );
    //OrbitingFrame::RegisterComponent( context );
    CameraFrame::RegisterComponent( context );
    Environment::RegisterComponent( context );

    MainMenu::RegisterObject( context );

    RepComp::RegisterComponent( context );
    StaticMesh::RegisterComponent( context );
    InfinitePlane::RegisterComponent( context );
    IcoPlanet::RegisterComponent( context );
    TestEnvironment::RegisterComponent( context );
}


}






