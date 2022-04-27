
#ifndef __MATERIAL_SOURCE_SCRIPT_H_
#define __MATERIAL_SOURCE_SCRIPT_H_

#include "volume_source.h"
#include "core/reference.h"

namespace Ign
{

class MaterialSourceScript: public MaterialSource
{
public:
	MaterialSourceScript();
	MaterialSourceScript();

public:
	Ref<Reference> object;
};


}






#endif




