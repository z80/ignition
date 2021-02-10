

#include "process_pid.h"
#include <unistd.h>

void ProcessPid::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("pid"),        &ProcessPid::pid );
    ClassDB::bind_method( D_METHOD("parent_pid"), &ProcessPid::parent_pid );
}

ProcessPid::ProcessPid()
{
}

ProcessPid::~ProcessPid()
{
}

int ProcessPid::pid()
{
    const pid_t pid = getpid();
    return static_cast<int>( pid );
}

int ProcessPid::parent_pid()
{
    const pid_t ppid = getppid();
    return static_cast<int>( ppid );
}









