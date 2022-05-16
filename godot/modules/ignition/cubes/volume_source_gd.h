
#ifndef __VOLUME_SOURCE_GD_H_
#define __VOLUME_SOURCE_GD_H_

#include "volume_source.h"
#include "marching_volume_object_gd.h"
#include "se3_ref.h"

namespace Ign
{

class VolumeSourceGd: public MarchingVolumeObjectGd
{
	GDCLASS(VolumeSourceGd, MarchingVolumeObjectGd);
protected:
	static void _bind_methods();

public:
	VolumeSourceGd();
	virtual ~VolumeSourceGd();

	real_t value( const Vector3 & at ) const;

	void set_inverted( bool en );
	bool get_inverted() const;

	real_t max_node_size() const;
	real_t min_node_size() const;
	real_t max_node_size_at( const Vector3 & at ) const;


	int material( const Vector3 & at );

	void set_material_only( bool only );
	bool get_material_only() const;

	void set_weak_material( bool weak );
	bool get_weak_material() const;


public:
	VolumeSource * source;
};




}





#endif




