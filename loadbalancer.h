#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <unistd.h> //process creation
#include <sys/wait.h>  //wait for childprocess
#include <sys/types.h>
#include <signal.h>   // For kill() to work
#include <ev.h>
#include <ofstream>
#include "definitions.h"
#include <iostream>
using namespace std;

class LoadBalancer
{
    public:
        void accept_balance( struct ev_loop* loop, struct ev_io* io, int r );
    public:
        void init();
        void start();
        void stop();
        void restart();
    public:
        void error( string );
        void info( string );

    public:
        LoadBalancer();
        virtual ~LoadBalancer();
    protected:
    private:
        int pSocket;
        int defaultPort;
        short numberOfChildren;
        struct sockaddr_in pServer;
        int maxPending;

        ofstream log;
        Process cProcess[3];
        string  cPath;
};

#endif // LOADBALANCER_H
