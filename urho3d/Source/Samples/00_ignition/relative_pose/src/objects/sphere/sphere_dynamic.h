
#ifndef __SPHERE_DYNAMIC_H_
#define __SPHERE_DYNAMIC_H_

#include "Urho3D/Urho3DAll.h"
#include "sphere_item.h"

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

    void setRadius( Float r );
    void setHeightSource( HeightSource * src );

    void subdriveLevelsInit() override;
protected:
    void applySourceCollision( Cubesphere & cs ) override;
    void applySourceVisual( Cubesphere & cs ) override;

    Float R_;
    HeightSource * height_source_;
};

}





#endif



