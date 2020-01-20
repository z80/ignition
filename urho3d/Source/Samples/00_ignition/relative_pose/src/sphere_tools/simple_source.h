
#ifndef __SIMPLE_SOURCE_H_
#define __SIMPLE_SOURCE_H_

#include "cube_sphere.h"

using namespace Urho3D;

namespace Ign
{

class SimpleSource: public HeightSource
{
public:
    SimpleSource();
    ~SimpleSource();

    Float height( const Vector3d & at ) const override;
    Color color( const Vector3d & at ) const override;
};

}






#endif







