#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "baseclient.h"
#include "clientconfig.h"
using namespace std;

int main()
{
    BaseClient *bc = new BaseClient("server_config.json", "client_config.json");
    string cmd, arg1, arg2, arg3;
    bool ctrl_flag[64];
    int state = 0;

    printf("| Simple client by 2015011313.\n");
    
    while(true)
    {
        state = 0;
        
        // First : login or register
        while(true)
        {
            cin >> cmd;
            //cout << (int)cmd.find("fastlogin") << " " << (int)cmd.find("login") <<endl;
            if ((int)cmd.find("fastlogin") > -1) break;
            else if ((int)cmd.find("login") > -1) {
                cin >> arg1 >> arg2 >> arg3;
                bc->getClientConfig()->account = arg1;
                bc->getClientConfig()->passwd = arg2;
                bc->getClientConfig()->name = arg3;
                break;
            }
        }
        printf("| Connecting to server.\n");

        state ++;
        ctrl_flag[state] = bc->connectServer();
        if(!ctrl_flag[state])
        {
            printf("| Connection to server failed.\n");
            continue;
        }
        printf("| Login to server.\n");
        
        state ++;
        ctrl_flag[state] = bc->login();
        if(!ctrl_flag[state])
        {
            printf("| Login to server failed.\n");
            continue;
        }

        printf("| Login ackwonledged.\n");

        bc->start_communication();
    }
    return 0;
}