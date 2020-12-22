
#ifndef __CUBE_VERTEX_H_
#define __CUBE_VERTEX_H_

#include "vector3d.h"

namespace Ign
{

struct Vertex
{
public:
    Vector3d at, atFlat, atUnit;
    Float heightUnit_;
    Vector3d norm;
    Color    color;

    int  a, b;
    bool isMidPoint;
    int  leafFacesQty;

    Vertex();
    ~Vertex();
    Vertex( const Vector3d & at );
    Vertex( const Vertex & inst );
    const Vertex & operator=( const Vertex & inst );
};



}






#endif






