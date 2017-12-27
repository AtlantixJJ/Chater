#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "baseserver.h"

#include "userdatabase.h"

int main ()
{
    UserDataBase *db = new UserDataBase("DataBase.json");
    db->recheck();
    BaseServer *server = new BaseServer("server_config.json");
    server->setDataBase(db);
    server->start_service();
    return 0;
}