#ifndef BASESERVER_H_
#define BASESERVER_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "serverconfig.h"
using namespace std;

class BaseServer
{
public:
    BaseServer(const ServerConfig &ref);
    BaseServer(std::string config_fname);
    ~BaseServer();

private:
    bool start_socket();

private:
    ServerConfig *sc;
    int sockfd;
    sockaddr_in addr;
};

#endif