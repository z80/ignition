
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
    context->RegisterFactory<KeplerMover>();

    URHO3D_ATTRIBUTE( "World R.x_", double, worldR_.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World R.y_", double, worldR_.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World R.z_", double, worldR_.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "World V.x_", double, worldV_.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World V.y_", double, worldV_.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World V.z_", double, worldV_.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "World W.x_", double, worldW_.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World W.y_", double, worldW_.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World W.z_", double, worldW_.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "World Q.w_", double, worldQ_.w_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World Q.x_", double, worldQ_.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World Q.y_", double, worldQ_.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "World Q.z_", double, worldQ_.z_, 0.0, AM_DEFAULT );
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


