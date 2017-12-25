#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "common.h"
#include "serverconfig.h"
#include "clientconfig.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

class BaseClient
{

friend class RecvStatus;
public:
    BaseClient(string fname);
    ~BaseClient();

    void connectServer();

    void start_communication();

private:
    static void* recv_thread(void *p);

private:
    ServerConfig *sc;
    ClientConfig *cc;

    int sockfd;
    struct sockaddr_in addr;
};

#endif