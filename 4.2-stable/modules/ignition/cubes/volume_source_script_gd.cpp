
#include "volume_source_script_gd.h"


namespace Ign
{

void VolumeSourceScriptGd::_bind_methods()
{
}


VolumeSourceScriptGd::VolumeSourceScriptGd()
	: VolumeSourceGd()
{
	source_script.reference = this;
	this->object = &source_script;
	this->source = &source_script;
}


VolumeSourceScriptGd::~VolumeSourceScriptGd()
{
}



}




