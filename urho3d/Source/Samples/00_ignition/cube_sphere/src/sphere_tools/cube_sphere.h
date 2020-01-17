
#ifndef __CUBE_SPHERE_H_
#define __CUBE_SPHERE_H_

#include "Urho3D/Urho3DAll.h"
#include "vector3d.h"

using namespace Urho3D;

namespace Ign
{

typedef double Float;
static const int EDGE_HASH_SZ = sizeof(int)*2;

class Cubesphere;
class Face;


class SubdriveSource
{
public:
    struct Level
    {
        Float sz;
        Float dist;
    };

    SubdriveSource();
    virtual ~SubdriveSource();

    void DrawDebugGeometry( float scale, DebugRenderer * debug, bool depthTest ) const;

    void setR( Float r );
    void clearLevels();
    void addLevel( Float sz, Float dist );

    // Based on changed interest points determines if 
    // need to recomute subdivision. 
    virtual bool needSubdrive( const Cubesphere * s, Vector<Vector3d> & pts );
    // Determines if a particular face should be subdriven.
    virtual bool needSubdrive( const Cubesphere * s, const Face * f ) const;

public:
    void flattenPts( const Cubesphere * s );

    Float r_;
    Vector<Level> levels_;
    Vector<Vector3d> pts_, ptsFlat_, ptsNew_;

    mutable bool done_;
};

class HeightSource
{
public:
    HeightSource();
    virtual ~HeightSource();

    virtual Float height( const Vector3d & at ) const = 0;
    virtual Color color( const Vector3d & at ) const = 0;
};

struct Vertex
{
public:
    Vector3d at, atFlat;
    Vector3d norm;

    int  a, b;
    bool isMidPoint;
    int  leafFacesQty;

    Vertex();
    ~Vertex();
    Vertex( const Vector3d & at );
    Vertex( const Vertex & inst );
    const Vertex & operator=( const Vertex & inst );
};

class Face
{
public:
    int  vertexInds[4];
    int  childInds[4];
    int  parentInd;
    int  indexInParent;
    int  level;
    bool leaf;

    Face();
    ~Face();

    void DrawDebugGeometry( float scale, const Cubesphere * s, DebugRenderer * debug, bool depthTest, bool flat=true ) const;

    Face( int a, int b, int c, int d );
    Face( const Face & inst );
    const Face & operator=( const Face & inst );
    bool subdrive( Cubesphere * s, SubdriveSource * src );

    const Vector3d normal( const Cubesphere * s ) const;
    const Float size( const Cubesphere * s ) const;
    static bool correctSide( const Vector3d & n, const Vector3d & a );
    // From 3d space to a face of a unit cube.
    static bool centralProjection( const Vector3d & n, const Vector3d & a, Vector3d & proj );
    // Vector "a" should be already projected on appropriate face.
    // E.i. "correctSide()" and "centralProjection()" should be already done.
    // "sz" allows to select all the faces within an area of size "sz".
    bool inside( const Cubesphere * s, const Vector3d & a, const Vector3d & n, const Float dist=0.1 ) const;
    bool selectLeafs( const Cubesphere * s, const Vector3d & a, const Float dist, Vector<int> & faceInds ) const;
};

class EdgeHash
{
public:
    unsigned char d[EDGE_HASH_SZ];

    EdgeHash();
    ~EdgeHash();
    EdgeHash( int a, int b );
    EdgeHash( const EdgeHash & inst );
    const EdgeHash & operator=( const EdgeHash & inst );
    unsigned ToHash() const;
    friend bool operator<( const EdgeHash & a, const EdgeHash & b );
    friend bool operator==( const EdgeHash & a, const EdgeHash & b );
};

class Cubesphere
{
public:
    Vector<Vertex>          verts;
    Vector<Face>            faces;
    HashMap<EdgeHash, int>  lookup;

    Cubesphere();
    ~Cubesphere();

    void DrawDebugGeometry( DebugRenderer * debug, bool depthTest, bool flat=true ) const;

    Cubesphere( const Cubesphere & inst );
    const Cubesphere & operator=( const Cubesphere & inst );

    void subdrive( SubdriveSource * src );
    void applySource( HeightSource * src );

    void triangleList( Vector<Vertex> & tris );

    // For selecting collision patches.
    void triangleList( const Vector<Vector3d> & pts, Vector<Vertex> & tris );
    void flattenPts( const Vector<Vector3d> & pts, Vector<Vector3d> & ptsFlat ) const;
private:
    void clear();
    void init();
    void labelMidPoints();
    void scaleToSphere();
    void computeNormals();
    void applySource( HeightSource * src, Vertex & v );

    void selectFaces( const Vector<Vector3d> & pts, const Float dist, Vector<int> & faceInds );

    Vector<Vector3d> ptsFlat_;
};

class CubeSphereComponent: public Component
{
    URHO3D_OBJECT( CubeSphereComponent, Component )
public:
    static void RegisterComponent( Context * context );

    CubeSphereComponent( Context * context );
    ~CubeSphereComponent();

    void DrawDebugGeometry( DebugRenderer * debug, bool depthTest ) override;

public:
    Vector<Vector3d> pts_;
    SubdriveSource subdriveSource_;
    Cubesphere     cubesphereCube_,
                   cubesphereSphere_;
};


}

#endif


