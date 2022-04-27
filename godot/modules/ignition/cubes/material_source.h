
#ifndef __MATERIAL_SOURCE_H_
#define __MATERIAL_SOURCE_H_

#include "data_types.h"
#include "vector3d.h"
#include "se3.h"

namespace Ign
{

class MaterialSource
{
public:
	MaterialSource();
	virtual ~MaterialSource();

	// This one computes value in global space.
	virtual int material( const Vector3d & at ) const;
};


}




#endif






