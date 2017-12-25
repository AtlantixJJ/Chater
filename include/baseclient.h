#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "serverconfig.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

class BaseClient
{
public:
    BaseClient(string fname);
    ~BaseClient();

private:
    void init();

private:
    ServerConfig *sc;

    int sockfd;
    struct sockaddr_in addr;
};

#endif