
#ifndef __MATERIAL_SOURCE_GD_H_
#define __MATERIAL_SOURCE_GD_H_

#include "core/reference.h"
#include "material_source.h"


namespace Ign
{

class MaterialSourceGd: public Reference
{
	GDCLASS(MaterialSourceGd, Reference);
protected:
	static void _bind_methods();
public:
	MaterialSourceGd();
	virtual ~MaterialSourceGd();

	virtual int material( const Vector3 & at );

public:
	MaterialSource * source;
};


}





#endif





