
#ifndef __REP_COMP_H_
#define __REP_COMP_H_

#include "Urho3D/Urho3DAll.h"


using namespace Urho3D;

namespace Ign
{

class RepComp: public Component
{
    URHO3D_OBJECT( RepComp, Component )
public:
    static void RegisterObject( Context * context );

    RepComp( Context * context );
    ~RepComp();

    void SetName( const String & name );
    const String & Name() const;

private:
    String name_;
};

}


#endif

