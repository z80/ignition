
#include "volume_source_script_gd.h"


namespace Ign
{

void VolumeSourceScriptGd::_bind_methods()
{
}


VolumeSourceScriptGd::VolumeSourceScriptGd()
	: VolumeSourceGd()
{
	this->source = &source_script;
	this->object = &source_script;
	source_script.object = Ref<VolumeSourceScriptGd>( this );
}


VolumeSourceScriptGd::~VolumeSourceScriptGd()
{
}



}




