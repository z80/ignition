
#ifndef __IGN_RANDOM_GD_H_
#define __IGN_RANDOM_GD_H_

#include "ign_random.h"
#include "core/object/ref_counted.h"


namespace Ign
{

class IgnRandomGd: public RefCounted
{
	GDCLASS(IgnRandomGd, RefCounted);
protected:
	static void _bind_methods();

public:
	IgnRandomGd();
	~IgnRandomGd();

	void set_seed( const String & stri );
	String get_seed() const;

	int integer();
	int integer_choices( int choices );
	int integer_interval_closed( int v_min, int v_max );

	real_t floating_point_top_open();
	real_t floating_point_closed();
	real_t floating_point_open();

	real_t floating_point_limit_open( real_t limit );
	real_t floating_point_limit_closed( real_t v_max );
	real_t floating_point_interval_closed( real_t v_min, real_t v_max );

	real_t normal( Float mean=0.0, Float stddev=1.0 );

	Vector3 random_vector( real_t length=1.0 );

	Quaternion random_rotation( const Vector3 & axis, real_t angle_variation, real_t axis_variation=0.0 );

public:
	IgnRandom rand;
};


}



#endif

