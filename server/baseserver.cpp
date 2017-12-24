#include "baseserver.h"
#include "serverconfig.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>

BaseServer::BaseServer(std::string config_fname)
{
    sc = new ServerConfig(config_fname);
    std::cout << "[BS] " << sc->toString() << std::endl;
    start_socket();
}

BaseServer::BaseServer(const ServerConfig &ref)
{
    this->sc = new ServerConfig(ref);
    std::cout << "[BS] " << sc->toString() << std::endl;
    start_socket();
}

BaseServer::~BaseServer()
{
    delete [] sc;
}

bool BaseServer::start_socket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("[init] Socket creation failed.");
        return false;
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(sc->getServerPort());
    addr.sin_addr.s_addr = inet_addr(sc->getServerIP());
    
    if (::bind(sockfd, (const sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("[init] Binding failed");
        exit(-1);
    }
}