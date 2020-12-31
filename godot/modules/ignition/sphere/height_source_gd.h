
#ifndef __HEIGHT_SOURCE_TEST_H_
#define __HEIGHT_SOURCE_TEST_H_

#include "height_source.h"


namespace Ign
{

class HeightSourceTest: public HeightSource
{
public:
	HeightSourceTest();
	~HeightSourceTest();

	Float height( const Vector3d & at ) const override;
	Color color( const Vector3d & at, const Vector3d & norm, Float height ) const override;
};

}




#endif





