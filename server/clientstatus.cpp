#include "clientstatus.h"
#include "common.h"

void ClientStatus::init()
{
    status = CLIENT_ONLINE;
}

void ClientStatus::disconnected()
{
    printf("[CS] %s disconnecting.\n", account.c_str());
    this->status = CLIENT_OFFLINE;
    if(account.length() > 0)
    {
        server_interface->db->setStatus(account, CLIENT_OFFLINE, this);
    }
    printf("[CS] %s Disconnected.\n", account.c_str());
}

void ClientStatus::verified(string ac)
{
    status = CLIENT_VERIFIED;
    account = ac;
    //printf("%s %d %ld\n", account.c_str(), CLIENT_VERIFIED, (long long)(this));
    server_interface->db->setStatus(account, CLIENT_VERIFIED, this);
}