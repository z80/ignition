
#include "register_types.h"

#include "core/class_db.h"
#include "frame_search.h"
#include "openvr_capture_node.h"

void register_mm_types()
{
	ClassDB::register_class<MM::FrameSearch>();
	ClassDB::register_class<OpenvrCaptureNode>();
}

void unregister_mm_types()
{
}


