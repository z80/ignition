
#include "register_types.h"

#include "core/class_db.h"
#include "frame_search.h"
#ifdef OPENVR_BUILD_STATIC
    #include "openvr_capture_node.h"
#endif

void register_mm_types()
{
    ClassDB::register_class<MM::FrameSearch>();
#ifdef OPENVR_BUILD_STATIC
    ClassDB::register_class<OpenvrCaptureNode>();
#endif
}

void unregister_mm_types()
{
}


