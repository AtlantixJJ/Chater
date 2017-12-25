#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "baseclient.h"
#include "clientconfig.h"

int main(){
    BaseClient *bc = new BaseClient("server_config.json");
    ClientConfig *cc = new ClientConfig("client_config.json");
    cc->writeToFile();
    return 0;
}