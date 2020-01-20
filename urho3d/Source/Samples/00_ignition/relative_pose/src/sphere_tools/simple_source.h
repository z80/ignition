
#ifndef __SIMPLE_SOURCE_H_
#define __SIMPLE_SOURCE_H_

#include "cube_sphere.h"


namespace Ign
{

class SimpleSource: public HeightSource
{
public:
    SimpleSource();
    ~SimpleSource();

    Float height( const Vector3d & at ) const;
    Color color( const Vector3d & at ) const;
};

}






#endif







