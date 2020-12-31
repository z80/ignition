
#include "height_source_gd.h"

namespace Ign
{

HeightSourceGd::HeightSourceGd()
	: HeightSource()
{
}

HeightSourceGd::~HeightSourceGd()
{
}

Float HeightSourceGd::height( const Vector3d & at ) const
{
	return 0.0;
}

Color HeightSourceGd::color( const Vector3d & at, const Vector3d & norm, Float height ) const
{
	Color c;
	c.r = 0.7;
	c.g = 0.0;
	c.b = 0.0;
	c.a = 1.0;
	return c;
}


}






