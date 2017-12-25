#ifndef COMMON_H_
#define COMMON_H_

#include "baseserver.h"

const int CLIENT_ONLINE     = 1;
const int CLIENT_OFFLINE    = CLIENT_ONLINE + 1;
const int CLIENT_UNINIT     = CLIENT_OFFLINE + 1;

class BaseServer;

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