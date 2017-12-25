#include "common.h"

void ClientStatus::init()
{
    status = CLIENT_ONLINE;
}

void RecvStatus::reset(int sockfd, BaseClient *bi)
{
    status = RECV_READY;
    this->sockfd = sockfd;
    this->client_interface = bi;
}