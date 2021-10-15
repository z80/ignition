
#ifndef __AIR_MESH_H_
#define __AIR_MESH_H_

#include "item_base.h"

using namespace Urho3D;

namespace Osp
{

struct Triangle
{
    Vector3d v[3]; // Vertices
    Vector3d n;    // Normal
    Vector3d at;   // Center
    Float   a;     // Area
};

class AirMesh
{
public:
    AirMesh();
    ~AirMesh();

    bool init( StaticModel * m );
    void drawDebugGeometry( Node * n, DebugRenderer * debug );

public:
    Vector<Triangle> triangles;
};

}


#endif



