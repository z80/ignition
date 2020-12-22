
#include "cube_vertex.h"


namespace Ign
{

Vertex::Vertex()
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
}

Vertex::~Vertex()
{

}

Vertex::Vertex( const Vector3d & at )
{
    a = b = -1;
    leafFacesQty = 0;
    isMidPoint   = false;
    this->atFlat = at;
    this->atUnit = at;
    this->at     = at;
}

Vertex::Vertex( const Vertex & inst )
{
    *this = inst;
}

const Vertex & Vertex::operator=( const Vertex & inst )
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





