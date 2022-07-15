
#include "cube_types.h"


namespace Ign
{

VectorInt operator+( const VectorInt & a, const VectorInt & b )
{
	const VectorInt ret = VectorInt( a.x+b.x, a.y+b.y, a.z+b.z );
	return ret;
}

bool operator<( const NodeFace & a, const NodeFace & b )
{
	const bool ret = (a.cell < b.cell);
	return ret;
}



}
