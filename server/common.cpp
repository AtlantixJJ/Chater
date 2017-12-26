#include "common.h"

void RecvStatus::reset(int sockfd, BaseClient *bi)
{
    status = RECV_READY;
    this->sockfd = sockfd;
    this->client_interface = bi;
}