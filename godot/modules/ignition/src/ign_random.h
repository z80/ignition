
#ifndef __IGN_RANDOM_H_
#define __IGN_RANDOM_H_

#include "pcg_random.h"
#include "data_types.h"
#include "vector3d.h"
#include "quaterniond.h"

namespace Ign
{

class IgnRandom: public PcgRandom
{
public:
	IgnRandom();
	virtual ~IgnRandom();

	IgnRandom( const IgnRandom & inst );
	const IgnRandom & operator=( const IgnRandom & inst );

	// [0, 2e32)
	int integer();
	// [0, choices)
	int integer_choices( int choices );
	// [v_min, v_max]
	int integer_interval_closed( int v_min, int v_max );

	// [0, 1)
	Float floating_point_top_open();
	// [0, 1]
	Float floating_point_closed();
	// (0, 1)
	Float floating_point_open();

	// [0, limit)
	Float floating_point_limit_open( Float limit );
	// [0, limit]
	Float floating_point_limit_closed( Float v_max );
	// [v_min, v_max]
	Float floating_point_interval_closed( Float v_min, Float v_max );

	// Normal distribution.
	Float normal( Float mean=0.0, Float stddev=1.0 );

	// Random vector.
	Vector3d random_vector( Float length=1.0 );

	// Random Rotation.
	Quaterniond random_rotation( const Vector3d & axis, Float angle_variation, Float axis_variation=0.0 );
};


}







#endif


