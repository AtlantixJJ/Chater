#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "serverconfig.h"

int main ()
{
    ServerConfig *sc = new ServerConfig("server_config.json");
    std::cout << sc->toString();
    return 0;
}