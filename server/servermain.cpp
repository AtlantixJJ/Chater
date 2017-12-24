#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "baseserver.h"

int main ()
{
    BaseServer *server = new BaseServer("server_config.json");

    return 0;
}