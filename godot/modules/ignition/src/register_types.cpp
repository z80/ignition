#include "register_types.h"

#include "core/class_db.h"
#include "occupancy_grid.h"

void register_ignition_types()
{
    ClassDB::register_class<OccupancyGrid>();
}

void unregister_ignition_types()
{
}
