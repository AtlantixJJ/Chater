#ifndef COMMON_H_
#define COMMON_H_

const int CLIENT_ONLINE     = 1;
const int CLIENT_OFFLINE    = CLIENT_ONLINE + 1;
const int CLIENT_UNINIT     = CLIENT_OFFLINE + 1;

class ClientStatus
{
public:
    ClientStatus() {
        sockfd = -1;
        status = CLIENT_UNINIT;
    }

    ClientStatus(int s) {
        sockfd = s;
    }

    void disconnected() {status = CLIENT_OFFLINE;}
    
    bool isOnline() const {return (status == CLIENT_ONLINE);}
    bool isAvailable() const {return (status == CLIENT_UNINIT);}
    int getSockfd() const {return sockfd;}

private:
    void init();

private:
    int sockfd;
    int status;
};


#endif 