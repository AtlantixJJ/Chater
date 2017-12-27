#ifndef BASECLIENT_H_
#define BASECLIENT_H_

#include "common.h"
#include "clientutils.h"
#include "serverconfig.h"
#include "clientconfig.h"
#include "cryptofile.h"

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
    void sendMessage(int op, const string& content);
    void sendMessage(const char *buf, int len);
    void sendFile(string fname);
    void start_recv();
    void start_chat();

    ClientConfig* getClientConfig() {return cc;}
    bool isChatting() const {return is_chatting;}
public:
    string peer_ac;
    Json::Value friends;
    Json::Value all_users;

private:
    static int decodeChatCMD(char *buf);
    void process_response(int op, string content);
    static void* recv_thread(void *p);

private:
    bool is_chatting;
    int process_state;

    ServerConfig *sc;
    ClientConfig *cc;

    int sockfd;
    struct sockaddr_in addr;
};

#endif