
#ifndef __VOLUME_SOURCE_SCRIPT_H_
#define __VOLUME_SOURCE_SCRIPT_H_

#include "volume_source.h"
#include "core/object/ref_counted.h"


namespace Ign
{

class VolumeSourceScript: public VolumeSource
{
public:
	VolumeSourceScript();
	virtual ~VolumeSourceScript();

	bool has_script() const;

	void set_bounding_radius( Float r ) override;
	Float get_bounding_radius() const override;

	Float value( const Vector3d & at );

	void set_inverted( bool en );
	bool get_inverted() const;

	Float max_node_size() const override;
	Float min_node_size() const override;
	Float max_node_size_local( const Vector3d & at ) override;
	Float min_node_size_local( const Vector3d & at ) override;

	int material( const Vector3d & at, int * priority = nullptr );


public:
	RefCounted * reference;
};


}

#endif








