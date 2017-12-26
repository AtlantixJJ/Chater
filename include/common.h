#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "baseclient.h"

const int CLIENT_ONLINE     = 1;
const int CLIENT_VERIFITED  = CLIENT_ONLINE + 1;
const int CLIENT_OFFLINE    = CLIENT_VERIFITED + 1;
const int CLIENT_UNINIT     = CLIENT_OFFLINE + 1;
const int RECV_READY        = CLIENT_UNINIT + 1;

const int CLIENT_MSG_WORD      = RECV_READY + 1;
const int CLIENT_MSG_REGISTER  = CLIENT_MSG_WORD + 1;
const int CLIENT_MSG_LOGIN     = CLIENT_MSG_REGISTER + 1;
const int CLIENT_MSG_ACK       = CLIENT_MSG_LOGIN + 1;
const int CLIENT_MSG_LIST      = CLIENT_MSG_ACK + 1;

inline void exitError(const char *msg)
{
    perror(msg);
    exit(-1);
}

class BaseClient;

class RecvStatus
{
public:
    int sockfd;
    int status;

    BaseClient *client_interface;

public:
    RecvStatus(int sockfd, BaseClient *bi){reset(sockfd, bi);}

private:
    void reset(int sockfd, BaseClient *bi);

};



#endif 