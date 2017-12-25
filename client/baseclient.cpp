#include "baseclient.h"

BaseClient::BaseClient(string fname)
{
    sc = new ServerConfig(fname);
    init();
}

BaseClient::~BaseClient()
{
    delete [] sc;
}

void BaseClient::init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(sc->getServerPort());
    addr.sin_addr.s_addr = inet_addr(sc->getServerIP());
    if (connect(sockfd,(sockaddr*)&addr,sizeof(addr)) == -1)
    {
        perror("[BC] Connecting to server failed.\n");
        exit(-1);
    }
    printf("[BC] Connection established.\n");
}