#ifndef COMMON_H_
#define COMMON_H_

#include "baseserver.h"

const int CLIENT_ONLINE     = 1;
const int CLIENT_OFFLINE    = CLIENT_ONLINE + 1;
const int CLIENT_UNINIT     = CLIENT_OFFLINE + 1;
const int RECV_READY        = CLIENT_UNINIT + 1;

class BaseServer;
class BaseClient;

struct message
{
    int msg_type;
    int data_size;
    char data[1024];

};

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

class ClientStatus
{
public:
    ClientStatus() {
        sockfd = -1;
        status = CLIENT_UNINIT;
        server_interface = nullptr;
    }

    ClientStatus(int s, BaseServer *si) {
        reset(s, si);
    }

    void reset(int s, BaseServer *si) {
        sockfd = s;
        status = CLIENT_ONLINE;
        server_interface = si;
    }

    BaseServer* getServerInterface() {return server_interface;}

    void disconnected() {status = CLIENT_OFFLINE;}
    
    bool isOnline() const {return (status == CLIENT_ONLINE);}
    bool isAvailable() const {return (status == CLIENT_UNINIT);}
    int getSockfd() const {return sockfd;}

private:
    void init();

private:
    int sockfd;
    int status;

    BaseServer *server_interface;
};

#endif 