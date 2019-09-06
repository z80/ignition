
#ifndef __THRUSTER_H_
#define __THRUSTER_H_

#include "block.h"

namespace Osp
{

class Thruster: public Block
{
    URHO3D_OBJECT( Thruster, Block )
public:
    Thruster( Context * c );
    ~Thruster();

    virtual void createContent( Node * node );
    virtual void toWorld();
    virtual void fromWorld();

    SharedPtr<StaticModel> visualModel;
};


}


#endif


