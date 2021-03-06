#ifndef BASESERVER_H_
#define BASESERVER_H_

#include "message.h"
#include "common.h"
#include "clientstatus.h"
#include "userdatabase.h"
#include "serverconfig.h"
#include "cryptofile.h"

using namespace std;

class ClientStatus;
class UserDataBase;

class BaseServer
{
friend class ClientStatus;
public:
    BaseServer(const ServerConfig &ref);
    BaseServer(std::string config_fname);
    ~BaseServer();

    void setDataBase(UserDataBase *d){db=d;}
    UserDataBase* getDataBase(){return db;}
    bool start_service();

    void process_message(ClientStatus *client, const char* buf);
    void setRecvAck(bool val){recv_ack = val;}
    bool getRecvAck(){return recv_ack;}
private:
    bool verify_passwd(ClientStatus *client, std::string content);
    bool register_user(ClientStatus *client, std::string content);
    void sendMessage(ClientStatus *client, int op, std::string content);
    void sendFile(ClientStatus *client, Message *msg);
    static void* sendFileThread(void *arg);
    void recvFile(Message *msg);
    static void* recvFileThread(void *arg);
private:
    bool init();
    bool start_socket();
    static int decodeCMD(std::string cmd);
    static void* cmdline_thread(void* args);
    static void* service_thread(void* args);

private:
    bool recv_ack;
    ServerConfig *sc;
    UserDataBase *db;
    ClientStatus *client_sockfd;

    int sockfd;
    struct sockaddr_in addr;
};

struct FileThreadInfo
{
    BaseServer *bs;
    Message *msg;
};

struct SendFileInfo
{
    BaseServer *bs;
    ClientStatus *client;
    string *fname;
};

#endif