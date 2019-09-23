
#include "kepler_mover.h"

namespace Ign
{

KeplerMover::KeplerMover( Context * ctx )
    : ItemNode( ctx )
{

}

KeplerMover::~KeplerMover()
{

}

void KeplerMover::IntegrateMotion( ItemNode * world, Timestamp dt )
{
    // Nothing by default.
}

void KeplerMover::ComputeRelativePose( ItemNode * world )
{
    // Nothing by default.
}

}


