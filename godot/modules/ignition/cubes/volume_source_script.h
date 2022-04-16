
#ifndef __VOLUME_SOURCE_SCRIPT_H_
#define __VOLUME_SOURCE_SCRIPT_H_

#include "volume_source.h"
#include "core/reference.h"


namespace Ign
{

class VolumeSourceScript: public VolumeSource
{
public:
	VolumeSourceScript();
	~VolumeSourceScript();

	bool has_script() const;

	Float value( const Vector3d & at ) const;

	void set_inverted( bool en );
	bool get_inverted() const;

	Float max_node_size() const;
	Float min_node_size() const;
	Float max_node_size_at( const Vector3d & at ) const;

public:
	Ref<Reference> object;
};


}

#endif








