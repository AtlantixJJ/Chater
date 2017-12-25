#include "baseclient.h"
#include "clientconfig.h"
#include "common.h"

BaseClient::BaseClient(string fname)
{
    sc = new ServerConfig(fname);
    cc = new ClientConfig(fname);
}

BaseClient::~BaseClient()
{
    delete [] sc;
    delete [] cc;
}

void BaseClient::connectServer()
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

void* BaseClient::recv_thread(void* p){
    RecvStatus *ptr = (RecvStatus*)p;
    while(1){
        char buf[100] = {};
        if (recv(ptr->sockfd, buf, sizeof(buf), 0) <= 0){
            return NULL;
        }
        printf("%s\n", buf);
    }
}

void BaseClient::start_communication()
{
    pthread_t id;
    RecvStatus *cs = new RecvStatus(sockfd, this);
    pthread_create(&id, 0, recv_thread, (void*)cs);
    char buf2[100] = {};
    sprintf(buf2, "%s进入了聊天室", cc->getName());
    send(sockfd, buf2, strlen(buf2), 0);
    while(1)
    {
        char buf[100] = {};
        scanf("%s", buf);
        char msg[131] = {};
        sprintf(msg, "%s:%s", cc->getName(), buf);
        send(sockfd, msg, strlen(msg), 0);
        if (strcmp(buf, "bye") == 0)
        {
            memset(buf2, 0, sizeof(buf2));
            sprintf(buf2, "%s退出了聊天室", cc->getName());
            send(sockfd, buf2, strlen(buf2), 0);
            break;
        }
    }
    close(sockfd);
}