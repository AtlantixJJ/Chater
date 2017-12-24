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
#include "common.h"
#include "serverconfig.h"
using namespace std;

class BaseServer
{
public:
    BaseServer(const ServerConfig &ref);
    BaseServer(std::string config_fname);
    ~BaseServer();

    bool start_service();

private:
    bool init();
    bool start_socket();
    static void* service_thread(void* args);
    void SendMsgToAll(char* msg);
private:
    ServerConfig *sc;

    ClientStatus *client_sockfd;

    int sockfd;
    struct sockaddr_in addr;
};

#endif