
#include "orbiting_frame.h"

namespace Ign
{

void OrbitingFrame::RegisterObject( Context * context )
{
}

OrbitingFrame::OrbitingFrame( Context * context )
    : EvolvingFrame( context )
{
}

OrbitingFrame::~OrbitingFrame()
{
}

void OrbitingFrame::evolveStep( Timestamp ticks_dt )
{
}

bool OrbitingFrame::Recursive() const
{
    return false;
}

bool OrbitingFrame::ProducesForces() const
{
    return true;
}

void OrbitingFrame::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
}

void OrbitingFrame::SetGM( Float GM )
{
}

Float OrbitingFrame::GM() const
{
    return GM_;
}

void OrbitingFrame::SetR( Float R )
{
}

Float OrbitingFrame::R() const
{
    return R_;
}

void OrbitingFrame::Launch( const Vector3d & r, const Vector3d & v )
{
}







}







