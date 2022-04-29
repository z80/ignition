
#ifndef __MATERIAL_SOURCE_SCRIPT_H_
#define __MATERIAL_SOURCE_SCRIPT_H_

#include "material_source.h"
#include "core/reference.h"

namespace Ign
{

class MaterialSourceScript: public MaterialSource
{
public:
	MaterialSourceScript();
	~MaterialSourceScript();

	bool has_script() const;

	int material( const Vector3d & at ) const;
public:
	Ref<Reference> object;
};


}






#endif




