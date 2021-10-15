
#ifndef __AIR_MESH_H_
#define __AIR_MESH_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"
#include "vector3d.h"

using namespace Urho3D;

namespace Ign
{

struct Triangle
{
    Vector3d v[3]; // Vertices
    Vector3d n;    // Normal
    Vector3d at;   // Center
    Float   a;     // Area
};

struct ForceApplied
{
    // Normal and lateral.
    Vector3d Fn, Fl, Pn, Pl;
    // Point where it is applied.
    Vector3d at;
};

class AirMesh
{
public:
    AirMesh();
    ~AirMesh();

    void operator=( StaticModel * m );
    bool init( StaticModel * m );
    void drawDebugGeometry( Node * n, DebugRenderer * debug );

public:
    Vector<Triangle> triangles_;
    Vector<ForceApplied> forces_;
};

}


#endif



