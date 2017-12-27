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
        server_interface->db->setStatus(account, CLIENT_OFFLINE);
    }
    printf("[CS] %s Disconnected.\n", account.c_str());
}

void ClientStatus::verified(string ac)
{
    status = CLIENT_VERIFITED;
    account = ac;
    server_interface->db->setStatus(account, CLIENT_VERIFITED);
}