#include "loadbalancer.h"

LoadBalancer::LoadBalancer()
{
    //open log file for data writes and append
    this->log.open("log.txt", ios::out|ios::app);
    this->cPath.append("./");
    this->cPath.append("processor");
    this->defaultPort = 56789;
    this->numberOfChildren = 3;
    this->maxPending = 100;
}

LoadBalancer::~LoadBalancer()
{
    //dtor
    this->log.close();
}

void LoadBalancer::start(){
    this->info("Starting load balancer...");
    this->init();
}

void LoadBalancer::init(){
    //read config file
    //parse config file
    //use config settings as required

    //create n named pipes for subprocess communication
    //create n subprocesses
    for( int i = 0; i < this->numberOfChildren; i++){
        //creating process first then the others//
        if( (this->cProcess[i].pid = fork()) < 0){
           this->error("Failed to create process, number -> " + i);
           this->error("Child Pipe creation failed, this may be due to lack of permission");
           this->error("Or unavailable system resources");
           exit(-1);
        }else if( this->cProcess[i].pid == 0){
            //process created successfully//
            if( pipe(this->cProcess[i].pipe) < 0) {
                this->error("Child Pipe creation failed, this may be due to lack of permission");
                this->error("Or unavailable system resources");
                exit(-1);
            }else{
                //pipe was created ok//
                //do stuff here//
                this->info("Child Process created successfully, PID ->" + getpid());
                execl(this->cPath.c_str(),NULL,NULL);
            }
        }else{
            this->info("Parent process" + getppid());
        }
    }

    //create and bind tcp socket to port from config file
    //listen for incomming connections
    //route to sub process base on who is less free
    //return and wait for more in comming requests

    //setup balancer socket
    this->pSocket = socket( PF_INET, SOCK_STREAM, IPPRTO_TCP);
    if( this->pSocket < 0) this->error("Failed to create socket");

    //setup server structure
    memset(&pServer, 0, sizeof(pServer));
    pServer.sin_family = AF_INET;
    pServer.sin_addr.s_addr = htonl(INADDR_ANY);
    pServer.sin_port = htons( this->defaultPort );

    //bind server to port
    if( bind(this->pSocket, (struct sockaddr)this->pServer, sizeof(this->pServer) ) < 0)
        this->error("Failed to bind server socket");
    //start listening if everything works fine to this stage
    if( listen( this->pSocket, this->maxPending) < 0)
        this->error("Failed to listen on server socket");

    //async listening with libev
    struct ev_io* io = malloc(sizeof(ev_io));
    ev_io_init(io, this->accept_balance, this->pSocket, EV_READ);
    ev_io_start(loop, io);

}

void LoadBalancer::accept_balance( struct ev_loop* loop, struct ev_io* io, int r ){

    // someone has connected. we accept the child.
    // then  push it to child process

    struct sockaddr_un client;
    socklen_t client_len = sizeof(client);
    int clientSocket = accept(this->pSocket, (sockaddr*)(&client), &client_len);

    //do load balancing here, pass off the connection descriptor to child process
    //which will in turn  create a read/write event on it
    int gt = 0;
    int pIndex = 0; //finding the process with least connections o process
    for( int p = 0; p < this->numberOfChildren; p ++){
        //init to first process in collection
        gt = this->cProcess[p].connections;
        pIndex = p;
        if( gt > this->cProcess[p].connections ){
            gt = this->cProcess[p].connections;
            pIndex = p;
        }
    }
    //once we found the process with least tasks to process
    //async listening with libev
    struct ev_io* io = malloc(sizeof(ev_io));
    ev_io_init(io, this->accept_balance, this->pSocket, EV_READ);
    ev_io_start(loop, io);

}

void LoadBalancer::error( string msg){
    this->log <<"ERROR"<<msg<<endl;
}

void LoadBalancer::info( string msg){
    this->log <<"INFO"<<msg<<endl;
}
