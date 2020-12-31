
#ifndef __HEIGHT_SOURCE_GD_H_
#define __HEIGHT_SOURCE_GD_H_

#include "height_source.h"
#include "core/reference.h"


namespace Ign
{

class HeightSourceGd: public HeightSource
{
public:
	HeightSourceGd();
	~HeightSourceGd();

	bool has_script() const;

	Float height( const Vector3d & at ) const override;
	Color color( const Vector3d & at, const Vector3d & norm, Float height ) const override;

	Ref<Reference> object;
};

}




#endif





