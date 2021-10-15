
#ifndef __CUBESPHERE_PLANET_H_
#define __CUBESPHERE_PLANET_H_

#include "cubesphere.h"
#include "planet_base.h"
#include "Urho3D/Urho3DAll.h"


namespace Osp
{

class PlanetCs: public PlanetBase, public Cubesphere::Source
{
    URHO3D_OBJECT( PlanetCs, PlanetBase )
public:
    PlanetCs( Context * ctx );
    ~PlanetCs();

    void setup( const String & json="Planets/Test.json" );

    Float dh( const Vector3d & at ) const override;
    bool  needSubdrive( const Cubesphere::Cubesphere * s, const Cubesphere::Face * f ) const override;

    void Start() override;
    void updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist ) override;
    void initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist ) override;
    void finitCollisions( PhysicsWorld2 * w2 ) override;

    bool load( const JSONValue & v ) override;
public:
    void initParameters( const String & fileName=String::EMPTY );
    void updateGeometry( Osp::WorldMover * mover );
    void updateGeometry( const Vector3d & at );
    Vector2 sphereCoord( const Vector3d & at ) const;

    String configFileName;
    bool   initialized;
    Cubesphere::Cubesphere cubesphere;
    // Visual representation.
    CustomGeometry * cg;
    SharedPtr<Image> heightmap,
                     colormap;
    Float   heightScale;
    int     subdivMaxLevel;
    Float   subdivMaxSine;
    // When dynamics is enabled
    RigidBody2      * rigidBody;
    CollisionShape2 * collisionShape;

    Vector<Cubesphere::Vertex> tris;
};


}


#endif






