
#ifndef __HEIGHT_SOURCE_H_
#define __HEIGHT_SOURCE_H_

#include "vector3d.h"
#include "core/color.h"

namespace Ign
{

class HeightSource
{
public:
    HeightSource();
    virtual ~HeightSource();

    virtual Float height( const Vector3d & at ) const = 0;
    virtual Color color( const Vector3d & at, const Vector3d & norm, Float height ) const = 0;
};


}


#endif





