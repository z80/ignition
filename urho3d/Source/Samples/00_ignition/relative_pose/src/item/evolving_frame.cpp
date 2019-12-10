
#include "evolving_frame.h"


namespace Ign
{

void EvolvingFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<RefFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

EvolvingFrame::EvolvingFrame( Context * context )
    : RefFrame( context )
{
}

EvolvingFrame::~EvolvingFrame()
{
}

void EvolvingFrame::evolveStep( Timestamp ticks_dt )
{
}

}




