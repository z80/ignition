
#ifndef __NUMERICAL_MOTION_H_
#define __NUMERICAL_MOTION_H_

#include "se3.h"
#include "celestial_consts.h"
#include "core/dictionary.h"

namespace Ign
{

class RefFrameNode;

class NumericalMotion
{
public:
	NumericalMotion();
	~NumericalMotion();

	void process( SE3 & se3, Float dt, const Vector3d & acc );

	Dictionary serialize() const;
	bool deserialize( const Dictionary & data );

public:
	Float time_step;

	bool debug;
};


}




#endif


