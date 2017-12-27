#include "baseclient.h"
#include "clientconfig.h"
#include "common.h"
#include "message.h"

BaseClient::BaseClient(string scf, string ccf)
{
    sc = new ServerConfig(scf);
    cc = new ClientConfig(ccf);
}

BaseClient::~BaseClient()
{
    delete [] sc;
    delete [] cc;
}

bool BaseClient::connectServer()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(sc->getServerPort());
    addr.sin_addr.s_addr = inet_addr(sc->getServerIP());
    if (connect(sockfd,(sockaddr*)&addr,sizeof(addr)) == -1)
    {
        printf("[BC] Connecting to server failed.\n");
        return false;
    }
    printf("[BC] Connection established.\n");
    return true;
}

void* BaseClient::recv_thread(void* p){
    RecvStatus *ptr = (RecvStatus*)p;
    while(1)
    {
        char buf[100] = {};
        if (recv(ptr->sockfd, buf, sizeof(buf), 0) <= 0){
            return NULL;
        }
        printf("%s\n", buf);
        sleep(1);
    }
}

bool BaseClient::register_account()
{
    Message *msg = new Message();
    char buf[2048];

    msg->type = CLIENT_MSG_REGISTER;
    msg->content = cc->getLoginContent();
    //printf("content:\n%s\n",msg->content.c_str());
    msg->encodeMessage();
    send(sockfd, msg->message, strlen(msg->message), 0);

    // wait for ack
    if(recv(sockfd, buf, sizeof(buf), 0) <= 0)
        exitError("[BC] Recv Ack failed.\n");

    printf("| Ack message recv : %s\n", buf);

    msg->fromBuffer(buf);
    msg->decodeMessage();
    if(msg->type != CLIENT_MSG_ACK)
        exitError("[BC] Recv type is not Ack.\n");
    
    if(msg->content == "1")
        return true;
    else
        return false;
}

bool BaseClient::sendRequest(int op)
{
    Message *msg = new Message();
    char buf[4096];

    msg->type = op;
    msg->content = "";
    msg->encodeMessage();
    send(sockfd, msg->message, strlen(msg->message), 0);
    if(recv(sockfd, buf, sizeof(buf), 0) <= 0)
        exitError("[BC] Recv response failed.\n");
    
    msg->fromBuffer(buf);
    msg->decodeMessage();
    printf("[BC] Response : %s\n", msg->message);
    
    return process_response(op, msg->content);
}

bool BaseClient::process_response(int op, string content)
{
    switch(op)
    {
    case CLIENT_MSG_SEARCH:
        all_users.clear();
        Json::Value node;std::istringstream stream(content);stream >> all_users;
        break;
    }
    
    return true;
}

bool BaseClient::login()
{
    Message *msg = new Message();
    char buf[2048];

    // send name, account and password
    msg->type = CLIENT_MSG_LOGIN;
    msg->content = cc->getLoginContent();
    //printf("content:\n%s\n",msg->content.c_str());
    msg->encodeMessage();
    send(sockfd, msg->message, strlen(msg->message), 0);

    // wait for ack
    if(recv(sockfd, buf, sizeof(buf), 0) <= 0)
        exitError("[BC] Recv Ack failed.\n");

    msg->fromBuffer(buf);
    msg->decodeMessage();
    if(msg->type != CLIENT_MSG_ACK)
        exitError("[BC] Recv type is not Ack.\n");

    if(msg->content == "1")
        return true;
    else
        return false;
}

void BaseClient::start_communication()
{
    pthread_t id;
    Message *msg = new Message();
    RecvStatus *cs = new RecvStatus(sockfd, this);

    pthread_create(&id, 0, recv_thread, (void*)cs);
    char buf2[100] = {};
    sprintf(buf2, "%s进入了聊天室", cc->getName());
    send(sockfd, buf2, strlen(buf2), 0);
    while(1)
    {
        char buf[1024] = {};
        scanf("%s", buf);
        
        msg->type = CLIENT_MSG_WORD;
        msg->content = buf;
        msg->encodeMessage();
        //printf("Encoded message : %s\n", msg->message);

        send(sockfd, msg->message, strlen(msg->message), 0);

        if (strcmp(buf, "bye") == 0)
        {
            memset(buf2, 0, sizeof(buf2));
            sprintf(buf2, "%s退出了聊天室", cc->getName());
            send(sockfd, buf2, strlen(buf2), 0);
            break;
        }

        
    }

    delete msg;
    close(sockfd);
}