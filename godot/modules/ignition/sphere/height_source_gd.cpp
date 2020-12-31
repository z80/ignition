
#include "height_source_test.h"

namespace Ign
{

HeightSourceTest::HeightSourceTest()
	: HeightSource()
{
}

HeightSourceTest::~HeightSourceTest()
{
}

Float HeightSourceTest::height( const Vector3d & at ) const
{
	return 0.0;
}

Color HeightSourceTest::color( const Vector3d & at, const Vector3d & norm, Float height ) const
{
	Color c;
	c.r = 0.7;
	c.g = 0.0;
	c.b = 0.0;
	c.a = 1.0;
	return c;
}


}






