
#include "material_source.h"

namespace Ign
{

MaterialSource::MaterialSource()
	: MarchingVolumeObject()
{
	strong = true;
}

MaterialSource::~MaterialSource()
{
}

int MaterialSource::material( const Vector3d & at ) const
{
    return 0;
}

void MaterialSource::set_strong( bool strong )
{
	this->strong = strong;
}

bool MaterialSource::get_strong() const
{
	return strong;
}


}


