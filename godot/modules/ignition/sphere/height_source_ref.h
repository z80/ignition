
#ifndef __HEIGHT_SOURCE_REF_H_
#define __HEIGHT_SOURCE_REF_H_

#include "core/object/ref_counted.h"


namespace Ign
{

class HeightSource;

class HeightSourceRef: public RefCounted
{
	GDCLASS( HeightSourceRef, RefCounted );
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	HeightSourceRef();
	virtual ~HeightSourceRef();

	// This is for usage inside gd script in the case if really needed.
	real_t height( const Vector3 & at ) const;
	Color  color( const Vector3 & at, const Vector3 & norm, real_t height ) const;

	// This one should be set by classes inherited from this one.
	HeightSource * height_source;
};


}


#endif


