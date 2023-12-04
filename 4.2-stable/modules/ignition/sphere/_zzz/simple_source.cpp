

#include "simple_source.h"


namespace Ign
{

SimpleSource::SimpleSource()
    : HeightSource()
{
}

SimpleSource::~SimpleSource()
{
}

Float SimpleSource::height( const Vector3d & at ) const
{
    return 0.0;
}

Color SimpleSource::color( const Vector3d & at, const Vector3d & norm, Float height ) const
{
    const Vector3d n( 0.0, 1.0, 0.0 );
    const Vector3d s( 0.0, -1.0, 0.0 );
    const Color c = at.DotProduct( n ) * Color::WHITE +
                    at.DotProduct( s ) * Color::RED;
    return c;
}


}










