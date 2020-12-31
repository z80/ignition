
#include "height_source_gd_ref.h"

namespace Ign
{

HeightSourceGdRef::HeightSourceGdRef()
	: HeightSourceRef()
{
	height_source = &height_source_gd;
	height_source_gd.object = Ref<HeightSourceGdRef>( this );
}

HeightSourceGdRef::~HeightSourceGdRef()
{
}



}






