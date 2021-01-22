
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
   
    virtual Float bounding_radius() const;
    void set_size( Float sz );
    Float get_size() const;
    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;

    Float size_;
};



}




#endif






