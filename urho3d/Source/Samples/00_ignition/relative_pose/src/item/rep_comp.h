
#ifndef __REP_COMP_H_
#define __REP_COMP_H_

#include "Urho3D/Urho3DAll.h"
#include "vector3d.h"

using namespace Urho3D;

namespace Ign
{

class RepComp: public Component
{
    URHO3D_OBJECT( RepComp, Component )
public:
    static void RegisterComponent( Context * context );

    RepComp( Context * context );
    ~RepComp();

    void SetName( const String & name );
    const String & Name() const;

    void SetPos( const Vector3d & p );
    const Vector3d & Pos() const;

private:
    String name_;
    Vector3d v_;
};

}


#endif

