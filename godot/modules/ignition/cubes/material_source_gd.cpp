
#include "material_source_gd.h"

namespace Ign
{

void MaterialSourceGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("material", "at"), &MaterialSourceGd::material, Variant::INT );
}

MaterialSourceGd::MaterialSourceGd()
	: MarchingVolumeObjectGd()
{
	source = nullptr;
}

MaterialSourceGd::~MaterialSourceGd()
{
}

int MaterialSourceGd::material( const Vector3 & at )
{
	if ( source == nullptr )
		return 0;

	const int ret = source->material( Vector3d( at.x, at.y, at.z ) );
	return ret;
}


}





