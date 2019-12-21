
#ifndef __INFINITE_PLAIN_H_
#define __INFINITE_PLAIN_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"

namespace Ign
{

class InfinitePlain: public RefFrame
{
    URHO3D_OBJECT( InfinitePlain, RefFrame )
public:
    void RegisterComponent( Context * context );

    InfinitePlain( Context * context );
    ~InfinitePlain();
};

}


#endif





