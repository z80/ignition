
#ifndef __HEIGHT_SOURCE_TEST_REF_H_
#define __HEIGHT_SOURCE_TEST_REF_H_

#include "height_source_ref.h"
#include "height_source_test.h"


namespace Ign
{

class HeightSourceTestRef: public HeightSourceRef
{
	GDCLASS( HeightSourceTestRef, HeightSourceRef );
public:
	HeightSourceTestRef();
	~HeightSourceTestRef();

	HeightSourceTest height_source_test;
};

}




#endif





