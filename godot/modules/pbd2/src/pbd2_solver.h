
#ifndef __PBD2_SOLVER_H_
#define __PBD2_SOLVER_H_

#include "pbd2_rigid_body.h"

namespace Pbd
{

namespace Solver
{

Float correct_position( Float h, Float compliance, Float c, const Vector3d & n, Float lambda, const Vector3d & r_a, RigidBody * body_a, const Vector3d & r_b, RigidBody * body_b );
Float correct_rotation( Float h, Float compliance, Float theta, const Vector3d & n, Float lambda, RigidBody * body_a, RigidBoy * body_b );

}



}




#endif





