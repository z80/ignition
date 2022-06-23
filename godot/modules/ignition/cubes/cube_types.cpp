
#include "cube_types.h"


namespace Ign
{

VectorInt operator+( const VectorInt & a, const VectorInt & b )
{
	const VectorInt ret = VectorInt( a.x+b.x, a.y+b.y, a.z+b.z );
	return ret;
}




}
