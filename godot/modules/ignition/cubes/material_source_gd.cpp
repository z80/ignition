
#include "material_source_gd.h"

namespace Ign
{

void MaterialSourceGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("material", "at"), &MaterialSourceGd::material, Variant::INT );
	ClassDB::bind_method( D_METHOD("set_weak", "en"), &MaterialSourceGd::set_weak );
	ClassDB::bind_method( D_METHOD("get_weak"),       &MaterialSourceGd::set_weak, Variant::BOOL );

	ADD_PROPERTY( PropertyInfo( Variant::BOOL, "weak" ), "set_weak", "get_weak" );
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

void MaterialSourceGd::set_weak( bool en )
{
	source->set_weak( en );
}

bool MaterialSourceGd::get_weak() const
{
	const bool ret = source->get_weak();
	return ret;
}




}





