
#ifndef __SPHERE_DYNAMIC_H_
#define __SPHERE_DYNAMIC_H_

#include "Urho3D/Urho3DAll.h"
#include "sphere_item.h"
#include "atmosphere_source.h"

using namespace Urho3D;

namespace Ign
{

class SphereDynamic: public SphereItem
{
    URHO3D_OBJECT( SphereDynamic, SphereItem )
public:
	/// Register object factory.
    static void RegisterComponent( Context * context );

	SphereDynamic( Context * context );
    ~SphereDynamic();

    /// Adjusting light direction here.
    void refStateChanged() override;

    // Implementation of ForceSourceFrame to
    // produce Coriolis and Centrifugal forces.
    bool Recursive() const override;
    bool ProducesForces() const override;
    void ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const override;

    // Sets body index in the system.
    // It is supposed to get height source and atmosphere source 
    // from SystemGenerator.
    int  GetBodyIndex() const;
    void SetBodyIndex( int index );

    void setRadius( Float r, Float h );
    void setHeightSource( HeightSource * src );
    void setAtmosphereSource( AtmosphereSource * src );
    void setStar( bool isStar );

    /// For assets placement returns 3d vector on a surface
    /// given a unit vector.
    Vector3d surfacePos( const Vector3d & unitAt, const Float height=0.0 );


    void subdriveLevelsInit() override;
protected:
    void initPiSurface();

    void applySourceCollision( Cubesphere & cs ) override;
    void applySourceVisual( Cubesphere & cs ) override;

    void ensureLight();
    void orientLight();

    int body_index_;
    Float R_, H_;
    bool is_star_;
    // In the case of a start it has a light.
    SharedPtr<Node>  light_node_;
    SharedPtr<Light> light_;
    HeightSource * height_source_;
    AtmosphereSource * atmosphere_source_;
};

}





#endif



