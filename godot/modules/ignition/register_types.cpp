#include "register_types.h"

#include "core/class_db.h"
#include "occupancy_grid.h"
#include "ref_frame.h"

void register_ignition_types()
{
    ClassDB::register_class<OccupancyGrid>();
	ClassDB::register_class<RefFrame>();
}

void unregister_ignition_types()
{
}
