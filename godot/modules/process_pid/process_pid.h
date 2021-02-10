
#ifndef __PROCESS_PID_H_
#define __PROCESS_PID_H_

#include "core/reference.h"


class ProcessPid: public Reference
{
    GDCLASS(ProcessPid, Reference);
protected:
    static void _bind_methods();
     
public:
     ProcessPid();
     ~ProcessPid();
     int pid();
     int parent_pid();
};



#endif





