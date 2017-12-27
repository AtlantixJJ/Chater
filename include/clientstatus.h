#ifndef CLIENTSTATUS_H_
#define CLIENTSTATUS_H_

#include "common.h"
#include "baseserver.h"
#include "message.h"

class BaseServer;

class ClientStatus
{
friend class BaseServer;
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

    void verified(string ac);
    //void unverified(string ac) {status = CLIENT_OFFLINE; account = ac;}
    BaseServer* getServerInterface() {return server_interface;}
    string getAccount()const{return account;}
    void disconnected();
    
    bool isOnline() const {return (status == CLIENT_ONLINE);}
    bool isAvailable() const {return (status == CLIENT_UNINIT);}
    int getSockfd() const {return sockfd;}

private:
    void init();
    string account;

private:
    int sockfd;
    int status;

    BaseServer *server_interface;
};

#endif 