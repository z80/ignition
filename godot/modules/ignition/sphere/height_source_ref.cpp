
#include "height_source_ref.h"
#include "height_source.h"
#include "vector3d.h"

namespace Ign
{
void HeightSourceRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("height", "at"),                  &HeightSourceRef::height, Variant::REAL );
	ClassDB::bind_method( D_METHOD("color", "at", "norm", "height"), &HeightSourceRef::color,  Variant::COLOR );
}

HeightSourceRef::HeightSourceRef()
	: Reference()
{
	height_source = nullptr;
}

HeightSourceRef::~HeightSourceRef()
{
}

real_t HeightSourceRef::height( const Vector3 & at ) const
{
	if ( height_source == nullptr )
		return 0.0;

	const Vector3d at_d( at.x, at.y, at.z );
	const Float h = height_source->height( at_d );
	return h;
}

Color  HeightSourceRef::color( const Vector3 & at, const Vector3 & norm, real_t height ) const
{
	if ( height_source == nullptr )
		return Color( 0.0, 0.0, 0.0 );

	const Vector3d at_d( at.x, at.y, at.z );
	const Vector3d norm_d( norm.x, norm.y, norm.z );
	const Float height_d = static_cast<Float>( height );
	const Color c = height_source->color( at_d, norm_d, height_d );

	return c;
}

}


