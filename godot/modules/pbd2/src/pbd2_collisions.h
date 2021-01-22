
#ifndef __PBD2_COLLISIONS_H_
#define __PBD2_COLLISIONS_H_

#include "pbd2_collision_object.h"

namespace Pbd
{

class CollisionSphere;
class CollisionBox;
class CollisionPlane;

void collision_sphere_sphere( CollisionSphere * obj_a, CollisionSphere * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );
void collision_sphere_plane( CollisionSphere * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );
void collision_sphere_box( CollisionSphere * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );
void collision_box_plane( CollisionBox * obj_a, CollisionPlane * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );
void collision_box_box( CollisionBox * obj_a, CollisionBox * obj_b, Vector<Vector3d> & ats, Vector<Vector3d> & depths );



}





#endif



