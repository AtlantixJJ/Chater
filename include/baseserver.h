#ifndef BASESERVER_H_
#define BASESERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "serverconfig.h"

class BaseServer
{
public:
    BaseServer(const ServerConfig &sc);
    BaseServer(string config_fname);
    ~BaseServer();

private:
    ServerConfig *sc;
};

#endif