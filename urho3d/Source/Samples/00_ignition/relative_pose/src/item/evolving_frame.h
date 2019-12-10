
#ifndef __EVOLVING_FRAME_H_
#define __EVOLVING_FRAME_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"

namespace Ign
{

class EvolvingFrame: public RefFrame
{
    URHO3D_OBJECT( EvolvingFrame, RefFrame )
public:
    static void RegisterComponent( Context * context );

    EvolvingFrame( Context * context );
    virtual ~EvolvingFrame();
    
    virtual void evolveStep( Timestamp ticks_dt );
};

}


#endif


