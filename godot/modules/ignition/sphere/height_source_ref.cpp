
#include "height_source_ref.h"

namespace Ign
{
void HeightSourceRef::_bind_methods()
{
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
	const real_t height_t = static_cast<real_t>( height );
	const Color c = height_source->color( at_d, norm_d, height_d ) const;

	return c;
}

}


