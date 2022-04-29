
#ifndef __MATERIAL_SOURCE_SCRIPT_GD_H_
#define __MATERIAL_SOURCE_SCRIPT_GD_H_

#include "material_source_gd.h"
#include "material_source_script.h"

namespace Ign
{

class MaterialSourceScriptGd: public MaterialSourceGd
{
	GDCLASS(MaterialSourceScriptGd, MaterialSourceGd);
protected:
	static void _bind_methods();

public:
	MaterialSourceScriptGd();
	virtual ~MaterialSourceScriptGd();

public:
	MaterialSourceScript material_source;
};

}






#endif










