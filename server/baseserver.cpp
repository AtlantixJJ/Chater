#include "baseserver.h"
#include "serverconfig.h"
#include "common.h"
#include "message.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>

bool BaseServer::init()
{
    db = nullptr;
    std::cout << "[BS] " << sc->toString() << std::endl;

    client_sockfd = new ClientStatus[sc->getMaxThread()];

    start_socket();
    return true;
}

BaseServer::BaseServer(std::string config_fname)
{
    sc = new ServerConfig(config_fname);
    init();
}

BaseServer::BaseServer(const ServerConfig &ref)
{
    this->sc = new ServerConfig(ref);
    init();
}

BaseServer::~BaseServer()
{
    delete [] sc;
    delete [] client_sockfd;
}

void* BaseServer::service_thread(void *p)
{
    ClientStatus *ptr = (ClientStatus*)p;
    printf("[BS][service=%d]\n", ptr->getSockfd());
    while(1)
    {
        char buf[1100] = {};
        if (recv(ptr->getSockfd(), buf, sizeof(buf), 0) <= 0 ||
            ptr->status == CLIENT_OFFLINE)
        {
            int i;
            ptr->status = CLIENT_OFFLINE;
            printf("[BS][service=%d] quit\n", ptr->getSockfd());
            pthread_exit((void*)ptr);
        }

        ptr->getServerInterface()->process_message(ptr, (const char*)buf);
    }
}

bool BaseServer::start_service()
{
    printf("[BS][service] Service start.\n");
    while(1)
    {
        struct sockaddr_in fromaddr;
        socklen_t len = sizeof(fromaddr);
        int fd = accept(sockfd, (sockaddr*)&fromaddr, &len);
        if (fd == -1)
        {
            printf("[BS] Accept failed...\n");
            continue;
        }
        int i = 0;
        for (i = 0;i < sc->getMaxThread();i++)
        {
            if (client_sockfd[i].isAvailable())
            {

                client_sockfd[i].reset(fd, this);
                printf("fd = %d\n", fd);

                pthread_t tid;
                pthread_create(&tid, 0, service_thread, (void*)(client_sockfd + i));
                break;
            }
            if (sc->getMaxThread() == i)
            {
                char* str = "Server full.";
                send(fd, str, strlen(str), 0); 
                close(fd);
            }
        }
    }
}

bool BaseServer::start_socket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("[BS][init] Socket creation failed.");
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(sc->getServerPort());
    addr.sin_addr.s_addr = inet_addr(sc->getServerIP());

    if (::bind(sockfd, (const sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("[BS][init] Binding failed.");
        exit(-1);
    }

    if (::listen(sockfd,100) == -1)
    {
        perror("[BS][init] Listen failed.");
        exit(-1);
    }

}

bool BaseServer::verify_passwd(ClientStatus *client, std::string content)
{
    Json::Value root;std::istringstream stream(content);stream >> root;
    std::cout << root <<std::endl;

    bool passed = db->loginVerify(root["account"].asString(), root["passwd"].asString());
    Message *ack = new Message();
    ack->type = CLIENT_MSG_ACK;

    if (!passed)
    {
        ack->content = "0";
        printf("[BS] Verification failed.\n");
    }
    else
    {
        ack->content = "1";
        printf("[BS] Verification succeed.\n");
    }

    ack->encodeMessage();
    printf("[BS] Send ack : %s", ack->message);

    send(client->getSockfd(), ack->message, strlen(ack->message), 0);
    delete ack;

    return true;
}

bool BaseServer::register_user(ClientStatus *client, std::string content)
{
    client->status = CLIENT_OFFLINE;
    Json::Value root;std::istringstream stream(content);stream >> root;

    db->registerUser(root["account"].asString(), root["passwd"].asString());

    Message *ack = new Message();
    ack->type = CLIENT_MSG_ACK;
    ack->content = "1";
    ack->encodeMessage();
    printf("[BS] Send ack : %s", ack->message);
    //std::cout << ack->root<< std::endl;
    send(client->getSockfd(), ack->message, strlen(ack->message), 0);
    delete ack;
    return true;
}

void BaseServer::process_message(ClientStatus *client, const char* buf)
{
    int i;
    Message *msg = new Message(buf);

    printf("[BS] Recv : %s\n", msg->message);
    msg->decodeMessage();

    switch(msg->type)
    {
    case CLIENT_MSG_LOGIN:
        verify_passwd(client, msg->content);
        break;
    case CLIENT_MSG_REGISTER:
        register_user(client, msg->content);
        break;
    case CLIENT_MSG_WORD:
        break;
    }

    delete msg;
}
