#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include <sys/types.h>

struct Process{
    pid_t pid;
    int connections;
    int pipe[2];
}


#endif // DEFINITIONS_H_INCLUDED
