#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "common.h"
#include "clientstatus.h"
#include "userdatabase.h"
#include "serverconfig.h"
using namespace std;

class ClientStatus;

class BaseServer
{
public:
    BaseServer(const ServerConfig &ref);
    BaseServer(std::string config_fname);
    ~BaseServer();

    void setDataBase(UserDataBase *d){db=d;}

    bool start_service();

    void process_message(ClientStatus *client, const char* buf);

private:
    bool verify_passwd(ClientStatus *client, std::string content);
    bool register_user(ClientStatus *client, std::string content);

private:
    bool init();
    bool start_socket();
    static void* service_thread(void* args);

private:
    ServerConfig *sc;
    UserDataBase *db;
    ClientStatus *client_sockfd;

    int sockfd;
    struct sockaddr_in addr;
};

#endif