
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "Urho3D/Urho3DAll.h"

using namespace Urho3D;

namespace Ign
{

class Environment: public LogicComponent
{
    URHO3D_OBJECT( Environment, LogicComponent )
public:
    Environment( Context * context );
    ~Environment();

    static void RedisterComponent( Context * context );


};

}


#endif


