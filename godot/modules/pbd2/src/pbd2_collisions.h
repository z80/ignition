
#ifndef __PBD2_COLLISIONS_H_
#define __PBD2_COLLISIONS_H_

#include "pbd2_collision_object.h"

namespace Pbd
{

class CollisionSphere;

void collision_sphere_sphere( CollisionSphere * obj_a, CollisionSphere * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );



}





#endif



