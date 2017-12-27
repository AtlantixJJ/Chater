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
        _vb(printf("[BC] Connecting to server failed.\n"));
        return false;
    }
    _vb(printf("[BC] Connection established.\n"));
    return true;
}

void* BaseClient::recv_thread(void* p){
    RecvStatus *ptr = (RecvStatus*)p;
    Message *msg = new Message();
    char buf[4096] = {};
    while(true)
    {
        if (recv(ptr->sockfd, buf, sizeof(buf), 0) <= 0){
            exitError("[BC] Server closed.\n");
        }
        msg->fromBuffer(buf);
        msg->decodeMessage();
        ptr->client_interface->process_response(msg->type, msg->content);
        usleep(1000);
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

void BaseClient::sendMessage(int op, string content)
{
    Message *msg = new Message();
    msg->type = op;
    msg->content = content;
    msg->encodeMessage();
    printf(" | [BC] Send %s\n", msg->message);
    send(sockfd, msg->message, strlen(msg->message), 0);
    delete msg;
}

bool BaseClient::sendRequest(int op, string content)
{
    sendMessage(op, content);
    process_state = -1;
    usleep(10);
    // stuck until response is answered
    while(process_state == -1)
        usleep(1000);

    return (process_state == 1);
}

/// Content is the message content
/// Usually a string or a JSON in decision message
void BaseClient::process_response(int op, string content)
{
    cout << " | [BC] Process : "<< content <<endl;
    bool flag;
    Json::Value respond;
    DecisionMessage *dmsg = new DecisionMessage();
    string spond;
    string smd;
    char cmd[128];

    std::ostringstream sstream(spond);
    std::istringstream stream(content);

    switch(op)
    {
    // Recv message of word
    case CLIENT_MSG_WORD:
        cout << peer_ac << " : " << endl << content << endl;
        break;
    // Recv content is account of peer
    case CLIENT_MSG_CHAT:
        peer_ac = content;
        is_chatting = true;
        break;
    // the server send search response back
    case CLIENT_MSG_SEARCH:
        all_users.clear();
        stream >> all_users;
        for (auto key : all_users.getMemberNames())
        {
            string trans;
            switch(all_users[key]["status"].asInt())
            {
            case 1: trans = "Online";break;
            case 2: trans = "Verified";break;
            case 3: trans = "Offline";break;
            }
            all_users[key]["status"] = trans; 
        }
        flag = true;
        break;
    // the server send a application for friend
    case CLIENT_MSG_APPADD:
        while(true)
        {
            printf(" | [BC] New friend request from user [%s], [y]es/[n]o?\n", content.c_str());
            scanf("%s", cmd);
            // accept : add friend and send ack
            if(cmd[0] == 'y' || cmd[0] == 'Y') {
                printf(" | [BC] Accept new friend.\n");
                friends[content] = all_users[content];
                dmsg->setDecision(content, "1");
                dmsg->encodeMessage();
                sendMessage(CLIENT_MSG_RESADD, dmsg->message);
                flag = true;
            } 
            else if (cmd[0] == 'n' || cmd[0] == 'N')
            {
                printf(" | [BC] Reject friend request.\n");
                flag = false;
                sendMessage(CLIENT_MSG_RESADD, "0");
            }
            else continue;
            break;
        }
        break;
    // the server send the result of application for friend back
    case CLIENT_MSG_RESADD:
        dmsg->fromBuffer(content.c_str());
        dmsg->decodeMessage();
        // Peer accepted, make new friends
        if (dmsg->dec == "1") {
            friends[dmsg->account] = all_users[dmsg->account];
            flag = true;
        } else if (dmsg->dec == "2") printf(" | [BC] Peer not online.\n"), flag = false;
        else flag = false;
        break;
    }
    
    if (flag) process_state = 1;
    else process_state = 0;
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

void BaseClient::start_recv()
{
    pthread_t id;
    RecvStatus *cs = new RecvStatus(sockfd, this);

    pthread_create(&id, 0, recv_thread, (void*)cs);
}

// s, f, e
int BaseClient::decodeChatCMD(char *buf)
{
    if (buf[0] == 's') return CLIENT_CMD_CHATMSG;
    if (buf[0] == 'f') return CLIENT_CMD_FILESEND;
    if (buf[0] == 'e') return CLIENT_CMD_EXIT;
}

void BaseClient::start_chat()
{
    char buf[4096];
    int op;

    while(true)
    {
        cin.getline(buf, 4096);
        op = BaseClient::decodeChatCMD(buf);

        switch(op)
        {
        case CLIENT_CMD_CHATMSG:
            sendMessage(CLIENT_MSG_WORD, string(buf+2));
            break;
        case CLIENT_CMD_FILESEND:
            break;
        case CLIENT_CMD_EXIT:
            sendMessage(CLIENT_MSG_CLOSECHAT, "");
            return;
            break;    
        }

    }
}