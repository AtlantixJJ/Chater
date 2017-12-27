#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "baseclient.h"
#include "json/json.h"

const int CLIENT_ONLINE     = 1;
const int CLIENT_VERIFIED  = CLIENT_ONLINE + 1;
const int CLIENT_OFFLINE    = CLIENT_VERIFIED + 1;
const int CLIENT_UNINIT     = CLIENT_OFFLINE + 1;
const int RECV_READY        = CLIENT_UNINIT + 1;
// send words in chatting
const int CLIENT_MSG_WORD       = RECV_READY + 1;
// send file in chatting
const int CLIENT_MSG_FILE       = CLIENT_MSG_WORD + 1;
const int CLIENT_MSG_REGISTER   = CLIENT_MSG_FILE + 1;
const int CLIENT_MSG_LOGIN      = CLIENT_MSG_REGISTER + 1;
const int CLIENT_MSG_ACK        = CLIENT_MSG_LOGIN + 1;
const int CLIENT_MSG_SEARCH     = CLIENT_MSG_ACK + 1;

// Application for adding a friend
const int CLIENT_MSG_APPADD     = CLIENT_MSG_SEARCH + 1;
// Response for friend application 
const int CLIENT_MSG_RESADD     = CLIENT_MSG_APPADD + 1;

// Start chatting with a friend
const int CLIENT_MSG_CHAT       = CLIENT_MSG_RESADD + 1;
const int CLIENT_MSG_CLOSECHAT  = CLIENT_MSG_CHAT + 1;

const int CLIENT_CMD_LIST       = CLIENT_MSG_CLOSECHAT + 1;
const int CLIENT_CMD_EXIT       = CLIENT_CMD_LIST + 1;
const int CLIENT_CMD_CHAT       = CLIENT_CMD_EXIT + 1;

const int CLIENT_CMD_CHATMSG    = CLIENT_CMD_CHAT+1;
const int CLIENT_CMD_SENDFILE   = CLIENT_CMD_CHATMSG + 1;

const int CLIENT_CMD_RECVPASTMSG= CLIENT_CMD_SENDFILE + 1;
const int CLIENT_CMD_RECVPASTFILE=CLIENT_CMD_RECVPASTMSG + 1;

const int SERVER_CMD_LISTDB     = CLIENT_CMD_RECVPASTFILE + 1;

inline void exitError(const char *msg)
{
    perror(msg);
    exit(-1);
}

class BaseClient;

class RecvStatus
{
public:
    int sockfd;
    int status;

    BaseClient *client_interface;

public:
    RecvStatus(int sockfd, BaseClient *bi){reset(sockfd, bi);}

private:
    void reset(int sockfd, BaseClient *bi);

};



#endif 