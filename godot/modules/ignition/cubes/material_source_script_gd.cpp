
#include "material_source_script_gd.h"


namespace Ign
{

void MaterialSourceScriptGd::_bind_methods()
{
}

MaterialSourceScriptGd::MaterialSourceScriptGd()
	: MaterialSourceGd()
{
	material_source.reference = this;
	this->source = &material_source;
	this->object = &material_source;
}

MaterialSourceScriptGd::~MaterialSourceScriptGd()
{
}



}









