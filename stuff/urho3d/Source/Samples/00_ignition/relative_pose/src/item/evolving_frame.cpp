
#include "evolving_frame.h"


namespace Ign
{

void EvolvingFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<ForceSourceFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( ForceSourceFrame );
}

EvolvingFrame::EvolvingFrame( Context * context )
    : ForceSourceFrame( context )
{
}

EvolvingFrame::~EvolvingFrame()
{
}

void EvolvingFrame::evolveStep( Timestamp ticks_dt )
{
}

}




