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
    db = nullptr;
    std::cout << "[BS] " << sc->toString() << std::endl;

    client_sockfd = new ClientStatus[sc->getMaxThread()];

    start_socket();
    pthread_t tid;
    pthread_create(&tid, 0, cmdline_thread, (void*)(this));
    return true;
}

int BaseServer::decodeCMD(std::string cmd)
{
    if((int)cmd.find("ld") > -1) return SERVER_CMD_LISTDB;
}

void* BaseServer::cmdline_thread(void *args)
{
    BaseServer *bs = (BaseServer*)args;
    std::string cmd;

    while(true)
    {
        std::cin >> cmd;
        switch(BaseServer::decodeCMD(cmd))
        {
        case SERVER_CMD_LISTDB:
            printf(" | [CMD] List current database:\n");
            printf(" | ----------------------------\n");
            std::cout << bs->getDataBase()->getRoot() << std::endl 
                << " | ----------------------------" << std::endl;
            break;
        default:
            printf(" | [CMD] Command \"%s\" not understand.\n", cmd.c_str());
        }
    }
    return NULL;
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
            ptr->disconnected();
            printf("[BS][service=%d] quit\n", ptr->getSockfd());
            pthread_exit((void*)ptr);
        }

        ptr->getServerInterface()->process_message(ptr, (const char*)buf);
    }
}

bool BaseServer::start_service()
{
    printf("[BS][service] Service start.\n");
    while(true)
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
    Json::Value node;std::istringstream stream(content);stream >> node;
    bool passed = db->loginVerify(node["account"].asString(), node["passwd"].asString());
    if (!passed)
    {
        sendMessage(client, CLIENT_MSG_ACK, "0");
        close(client->getSockfd());
        printf("[BS] Verification failed.\n");
    }
    else
    {
        sendMessage(client, CLIENT_MSG_ACK, "1");
        client->verified(node["account"].asString());
        printf("[BS] Verification succeed.\n");
    }

    return true;
}

bool BaseServer::register_user(ClientStatus *client, std::string content)
{
    Json::Value root;std::istringstream stream(content);stream >> root;

    db->registerUser(root["account"].asString(),
        root["passwd"].asString(), root["name"].asString());

    sendMessage(client, CLIENT_MSG_ACK, "1");

    client->disconnected();
    close(client->getSockfd());
    return true;
}

void BaseServer::sendMessage(ClientStatus *client, int op, std::string content)
{
    //cout << "S " << op << " " << content << endl;
    Message *ack = new Message();
    ack->type = op;
    ack->content = content;

    ack->encodeMessage();

    printf("[BS] Send to %d : %s\n", client->getSockfd(), ack->message);
    send(client->getSockfd(), ack->message, strlen(ack->message), 0);
    delete ack;
}

void BaseServer::sendFile(ClientStatus *client, Message *msg)
{
    CryptoFile cy;
    string fname = msg->root["filename"].asString();
    cy.decodeString(msg->content);
    cy.saveToFile(sc->download_path + fname);
}

void BaseServer::process_message(ClientStatus *client, const char* buf)
{
    int i;
    Message *msg = new Message(buf);
    Json::Value peer;// = NULL;
    DecisionMessage *dmsg = new DecisionMessage();
    ClientStatus *peer_cc = NULL;
    string str;

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
    // Chat words
    case CLIENT_MSG_WORD:
        peer = db->getRoot()[client->getAccount()]["peer"];
        peer_cc = (ClientStatus*)peer["status"].asInt64();
        sendMessage(peer_cc, CLIENT_MSG_WORD, msg->content);
        break;
    // Chat files
    case CLIENT_MSG_FILE:
        peer = db->getRoot()[client->getAccount()]["peer"];
        peer_cc = (ClientStatus*)peer["status"].asInt64();
        sendMessage(peer_cc, CLIENT_MSG_WORD, msg->message);
        break;
    // content is peer name
    case CLIENT_MSG_CHAT:
        peer = db->findUser(msg->content);
        peer_cc = (ClientStatus*)peer["client_status"].asInt64();
        sendMessage(peer_cc, CLIENT_MSG_CHAT, client->getAccount());
        break;
    case CLIENT_MSG_SEARCH:
        str = db->getAllUsers();
        printf("[BS] All users : %s\n", str.c_str());
        sendMessage(client, msg->type, str);
        break;
    // A client apply to add friend
    case CLIENT_MSG_APPADD:
        peer = db->findUser(msg->content);
        peer_cc = (ClientStatus*)peer["client_status"].asInt64();
        cout << "Content" << msg->content << "\nPeer: " << peer << endl;
        if (peer_cc != NULL && peer["status"].asInt() == CLIENT_VERIFIED)
            // send for acception
            sendMessage(peer_cc, CLIENT_MSG_APPADD, client->getAccount());
        else // 2 for client not online
            sendMessage(client, CLIENT_MSG_RESADD, "2");
        break;
    // A client responses add friend request
    case CLIENT_MSG_RESADD:
        dmsg->fromBuffer(msg->content.c_str());
        dmsg->decodeMessage();
        peer = db->findUser(dmsg->account);
        peer_cc = (ClientStatus*)peer["client_status"].asInt64();
        cout << "dmsg " << dmsg->message << "\nPeer: " << peer << endl;

        if (peer_cc != NULL && peer["status"].asInt() == CLIENT_VERIFIED)
        {
            if(dmsg->dec == "1") {
                // set peer relation in DB
                db->setPeer(dmsg->account, client->getAccount(), client);
                db->setPeer(client->getAccount(), dmsg->account, peer_cc);
                dmsg->setDecision(client->getAccount(), "1");
            } else
                dmsg->setDecision(client->getAccount(), "0");
                
            dmsg->encodeMessage();
            sendMessage(peer_cc, CLIENT_MSG_RESADD, dmsg->message);
        }
        else
            printf(" | [BS] Peer not online.\n");
        break;
    }

    delete msg;
    delete dmsg;
}
