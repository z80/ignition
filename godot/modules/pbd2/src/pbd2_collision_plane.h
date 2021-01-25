
#ifndef __PBD2_COLLISION_PLANE_H_
#define __PBD2_COLLISION_PLANE_H_

#include "pbd2_collision_object.h"

namespace Pbd
{

class CollisionPlane: public CollisionObject
{
public:
    CollisionPlane();
    ~CollisionPlane();
   
    Float bounding_radius() const;
    bool inside( const BroadTreeNode * n, Float h ) const override;
    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;
};



}




#endif






