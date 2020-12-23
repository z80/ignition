
#include "cube_vertex.h"


namespace Ign
{

CubeVertex::CubeVertex()
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
}

CubeVertex::~CubeVertex()
{

}

CubeVertex::CubeVertex( const Vector3d & at )
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
    this->atFlat = at;
    this->atUnit = at;
    this->at     = at;
}

CubeVertex::CubeVertex( const CubeVertex & inst )
{
    *this = inst;
}

const CubeVertex & CubeVertex::operator=( const CubeVertex & inst )
{
    if ( this != &inst )
    {
        atFlat  = inst.atFlat;
        atUnit  = inst.atUnit;
        at   = inst.at;
        heightUnit_ = inst.heightUnit_;
        norm = inst.norm;
        a    = inst.a;
        b    = inst.b;
        leafFacesQty = inst.leafFacesQty;
        isMidPoint   = inst.isMidPoint;
        color        = inst.color;
    }
    return *this;
}


}





