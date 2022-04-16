
#ifndef __VOLUME_SOURCE_GD_H_
#define __VOLUME_SOURCE_GD_H_

#include "volume_source.h"
#include "core/reference.h"
#include "se3_ref.h"

namespace Ign
{

class VolumeSourceGd: public Reference
{
	GDCLASS(VolumeSourceGd, Reference);
protected:
	static void _bind_methods();

public:
	VolumeSourceGd();
	virtual ~VolumeSourceGd();

	real_t value( const Vector3 & at ) const;

	void set_se3( const Ref<Se3Ref> & se3 );
	Ref<Se3Ref> get_se3() const;

	void set_inverted( bool en );
	bool get_inverted() const;

	real_t max_node_size() const;
	real_t min_node_size() const;
	real_t max_node_size_at( const Vector3 & at ) const;

public:
	VolumeSource * source;
};




}





#endif




