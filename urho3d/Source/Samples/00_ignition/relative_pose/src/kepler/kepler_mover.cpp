
#include "kepler_mover.h"

namespace Ign
{

const Matrix3d KeplerMover::TO_SPACE( 1.0, 0.0, 0.0,
                                      0.0, 0.0, 1.0,
                                      0.0, 1.0, 0.0 );
const Matrix3d KeplerMover::TO_GAME( 1.0, 0.0, 0.0,
                                     0.0, 0.0, 1.0,
                                     0.0, 1.0, 0.0 );


KeplerMover::KeplerMover( Context * ctx )
    : ItemNode( ctx )
{

}

KeplerMover::~KeplerMover()
{

}

void KeplerMover::RegisterObject( Context * context)
{
    context->RegisterFactory<KeplerTimeCounter>();
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


