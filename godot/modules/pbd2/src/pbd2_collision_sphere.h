
#ifndef __PBD2_COLLISION_SPHERE_H_
#define __PBD2_COLLISION_SPHERE_H_

#include "pbd2_collision_object.h"

namespace Pbd
{

class CollisionSphere: public CollisionObject
{
public:
    CollisionSphere();
    ~CollisionSphere();
    
    Float bounding_radius() const override;
    bool inside( const BroadTreeNode * n, Float h ) const override;
    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;


    Float radius;
};



}




#endif






