
#include "ref_frame_register.h"

#include "ref_frame.h"
#include "physics_frame.h"
#include "physics_item.h"
#include "evolving_frame.h"
#include "rotating_frame.h"
#include "orbiting_frame.h"
#include "environment.h"

#include "main_menu.h"

#include "rep_comp.h"
#include "static_mesh.h"

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
    Environment::RegisterComponent( context );

    MainMenu::RegisterComponent( context );
}


}






