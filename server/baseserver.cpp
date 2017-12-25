#include "baseserver.h"
#include "serverconfig.h"
#include "common.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>

bool BaseServer::init()
{
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
        if (recv(ptr->getSockfd(), buf, sizeof(buf), 0) <= 0)
        {
            int i;
            
            printf("[BS][service=%d] quit\n", ptr->getSockfd());
            pthread_exit((void*)i);
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
                //记录客户端的socket
                client_sockfd[i].reset(fd, this);
                printf("fd = %d\n", fd);
                //有客户端连接之后，启动线程给此客户服务
                pthread_t tid;
                pthread_create(&tid, 0, service_thread, (void*)(client_sockfd + i));
                break;
            }
            if (sc->getMaxThread() == i)
            {
                //发送给客户端说聊天室满了
                char* str = "对不起，聊天室已经满了!";
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

void BaseServer::process_message(ClientStatus *client, const char* msg)
{
    int i;
    //message *m = (message*)msg;

    for (i = 0;i < sc->getMaxThread();i++)
    {
        if (client_sockfd[i].isOnline())
        {
            printf("sendto%d\n", client_sockfd[i]);
            send(client_sockfd[i].getSockfd(), msg, strlen(msg), 0);
        }
    }
}
