
#ifndef __CELESTIAL_ROTATION_H_
#define __CELESTIAL_ROTATION_H_

#include "celestial_consts.h"
#include "se3.h"

namespace Ign
{

class CelestialRotation
{
public:
	CelestialRotation();
	~CelestialRotation();

	void init( const Vector3d & up, Float period_hrs );
	const SE3 & process( Float dt );

	Celestial::Ticks period;
	Celestial::Ticks time;
	Quaterniond      axis_orientation;
	SE3              se3;
};




}




#endif






