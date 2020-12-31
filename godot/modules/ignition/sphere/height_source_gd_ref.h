
#ifndef __HEIGHT_SOURCE_GD_REF_H_
#define __HEIGHT_SOURCE_GD_REF_H_

#include "height_source_ref.h"
#include "height_source_gd.h"


namespace Ign
{

class HeightSourceGdRef: public HeightSourceRef
{
	GDCLASS( HeightSourceGdRef, HeightSourceRef );
	OBJ_CATEGORY("Ignition");

public:
	HeightSourceGdRef();
	~HeightSourceGdRef();

	HeightSourceGd height_source_gd;
};

}




#endif





