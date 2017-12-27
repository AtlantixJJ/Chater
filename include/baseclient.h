#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "common.h"
#include "clientutils.h"
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
    BaseClient(string scf, string ccf);
    ~BaseClient();

    bool connectServer();

    bool login();
    bool register_account();
    bool sendRequest(int op, string content);
    void sendMessage(int op, string content);
    void start_recv();
    void start_chat();

    ClientConfig* getClientConfig() {return cc;}

public:
    Json::Value friends;
    Json::Value all_users;

private:
    
    void process_response(int op, string content);
    static void* recv_thread(void *p);

private:
    int process_state;

    ServerConfig *sc;
    ClientConfig *cc;

    int sockfd;
    struct sockaddr_in addr;
};

#endif