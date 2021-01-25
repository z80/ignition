
#ifndef __PBD2_COLLISION_BOX_H_
#define __PBD2_COLLISION_BOX_H_

#include "pbd2_collision_object.h"

namespace Pbd
{

class CollisionBox: public CollisionObject
{
public:
    CollisionBox();
    ~CollisionBox();
    
    Float bounding_radius() const override;
    bool inside( const BroadTreeNode * n, Float h ) const override;
    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;

    void set_size( const Vector3d & sz );
    Vector3d get_size() const;
	Vector3d get_size2() const;


    Vector3d size2;
};



}




#endif






