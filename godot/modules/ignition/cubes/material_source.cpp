
#include "material_source.h"

namespace Ign
{

MaterialSource::MaterialSource()
	: MarchingVolumeObject()
{
	weak = true;
}

MaterialSource::~MaterialSource()
{
}

int MaterialSource::material( const Vector3d & at ) const
{
    return 0;
}

void MaterialSource::set_weak( bool weak )
{
	this->weak = weak;
}

bool MaterialSource::get_weak() const
{
	return weak;
}


}


