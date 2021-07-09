
#ifndef __CUBE_VERTEX_H_
#define __CUBE_VERTEX_H_

#include "vector3d.h"
#include "core/color.h"

namespace Ign
{

struct CubeVertex
{
public:
    Vector3d at, atFlat, atUnit, atScaled;
    Float heightUnit_;
    Vector3d norm;
    Color    color;

    int  a, b;
    bool isMidPoint;
    int  leafFacesQty;

    CubeVertex();
    ~CubeVertex();
    CubeVertex( const Vector3d & at );
    CubeVertex( const CubeVertex & inst );
    const CubeVertex & operator=( const CubeVertex & inst );
};



}






#endif






