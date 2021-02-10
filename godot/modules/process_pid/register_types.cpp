/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "process_pid.h"

void register_process_pid_types()
{
    ClassDB::register_class<ProcessPid>();
}

void unregister_process_pid_types()
{
    //nothing to do here
}
