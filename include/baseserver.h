#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "common.h"
#include "clientstatus.h"
#include "serverconfig.h"
using namespace std;

class ClientStatus;

class BaseServer
{
public:
    BaseServer(const ServerConfig &ref);
    BaseServer(std::string config_fname);
    ~BaseServer();

    bool start_service();

    void process_message(ClientStatus *client, const char* buf);
private:
    bool init();
    bool start_socket();
    static void* service_thread(void* args);
private:
    ServerConfig *sc;

    ClientStatus *client_sockfd;

    int sockfd;
    struct sockaddr_in addr;
};

#endif