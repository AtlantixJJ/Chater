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

void* BaseClient::recv_thread(void* p)
{
    RecvStatus *ptr = (RecvStatus*)p;
    
    char buf[ENC_BLOCK_SIZE] = {};
    while(true)
    {
        if (recv(ptr->sockfd, buf, sizeof(buf), 0) <= 0)
        {
            exitError("[BC] Server closed.\n");
        }
        ptr->client_interface->process_response(buf);
        usleep(10);
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

void BaseClient::sendMessage(int op, const string& content)
{
    Message *msg = new Message();
    msg->type = op;
    msg->content = content;
    msg->encodeMessage();
    _vb(printf(" | [BC] Send %s\n", msg->message));
    send(sockfd, msg->message, strlen(msg->message), 0);
    delete msg;
}

void BaseClient::sendMessage(const char *buf, int len)
{
    _vb(printf(" | [BC] Send %s\n", buf));
    send(sockfd, buf, len, 0);
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

void BaseClient::recvFile(Message *msg)
{
    std::string fname = msg->root["filename"].asString();
    std::string data = msg->root["content"].asString();
    printf("Recv file(%lu/%lu)\n", data.length(), strlen(msg->message));
    CryptoFile cy;
    cy.openOutputFile("./downloads/" + fname);
    cy.writeFileSegment(data, 0, 0);
    cy.closeOutputFile();
}

void* BaseClient::recvFileThread(void *arg)
{
    //FileThreadInfo *info = (FileThreadInfo*)arg;
    Message *msg = (Message*)arg;
    msg->decodeMessage();
    std::string fname = msg->root["filename"].asString();
    std::string data = msg->root["content"].asString();
    printf("Recv file(%lu/%lu)\n", data.length(), strlen(msg->message));
    CryptoFile cy;
    cy.openOutputFile("./downloads/" + fname);
    cy.writeFileSegment(data, 0, 0);
    cy.closeOutputFile();
    delete msg;
    return NULL;
}

/// Content is the message content
/// Usually a string or a JSON in decision message
void BaseClient::process_response(const char *buf)
{
    bool flag;
    Message *msg = new Message();
    Json::Value respond;
    DecisionMessage *dmsg = new DecisionMessage();
    string spond;
    string smd;
    char cmd[128];

    msg->fromBuffer(buf);
    msg->decodeMessage();

    int op = msg->type;
    string content = msg->content;
    std::ostringstream sstream(spond);
    std::istringstream stream(content);

    switch(op)
    {
    // Recv file ack
    case CLIENT_MSG_FILEACK:
        recv_ack = true;
        break;
    // Recv file
    case CLIENT_MSG_FILE:
        recvFile(msg);
        sendMessage(CLIENT_MSG_FILEACK, "");
        break;
    // Recv message of word
    case CLIENT_MSG_WORD:
        cout << all_users[peer_ac]["name"] << " : " << endl << content << endl;
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
        } else if (dmsg->dec == "2") {
            printf(" | [BC] Peer not online.\n");
            flag = false;
        } else flag = false;
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
    if (buf[0] == 'f') return CLIENT_CMD_SENDFILE;
    if (buf[0] == 'e') return CLIENT_CMD_EXIT;
}

void BaseClient::sendFile(string fname)
{
    CryptoFile cy;
    cy.openFile(fname);
    int block_size = FILE_BLOCK_SIZE;
    int file_size = cy.getFileSize();
    int block_num = cy.getFileSize() / block_size;
    int pos = 0;
    if (block_num % block_size != 0) block_num ++;

    std::ostringstream stream;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding

    Json::Value msg_json;
    msg_json["type"]     = Json::Value(CLIENT_MSG_FILE  );
    msg_json["filename"] = Json::Value(fname            );

    for (int i = 0; i < block_num; i++)
    {
        recv_ack = false;
        pos = block_size * i;
        if (i == block_num - 1)
        {
            block_size = file_size % block_size;
            msg_json["eof"] = Json::Value(1);
        } else msg_json["eof"] = Json::Value(0);
        string seg = cy.getFileSegment(pos, block_size);
        msg_json["content"]     = Json::Value(seg              );
        msg_json["block_size"]  = Json::Value(block_size       );
        std::string document = Json::writeString(wbuilder, msg_json) + "\n\0\0";
        printf("Send File (%d/%d) : %s\n", seg.length(), document.length(), document.c_str());
        sendMessage(document.c_str(), document.length());
        while(!recv_ack) usleep(10);
    }
    cy.closeFile();
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
            cout << cc->getName() << " : " << endl;
            cout << buf + 2 << endl;
            sendMessage(CLIENT_MSG_WORD, string(buf+2));
            break;
        case CLIENT_CMD_SENDFILE:
            /// TODO: decide if the file exist
            cout << cc->getName() << " : [Send File] " << buf + 2 << endl;
            sendFile(buf+2);
            break;
        case CLIENT_CMD_EXIT:
            sendMessage(CLIENT_MSG_CLOSECHAT, "");
            return;
            break;    
        }

    }
}