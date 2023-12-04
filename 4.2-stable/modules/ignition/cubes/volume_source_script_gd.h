
#ifndef __VOLUME_SOURCE_SCRIPT_GD_H_
#define __VOLUME_SOURCE_SCRIPT_GD_H_

#include "volume_source_gd.h"
#include "volume_source_script.h"

namespace Ign
{

class VolumeSourceScriptGd: public VolumeSourceGd
{
	GDCLASS(VolumeSourceScriptGd, VolumeSourceGd);
protected:
	static void _bind_methods();

public:
	VolumeSourceScriptGd();
	~VolumeSourceScriptGd();

public:
	VolumeSourceScript source_script;
};



}



#endif




