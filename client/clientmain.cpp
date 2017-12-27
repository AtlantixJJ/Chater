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

int decodeCMD(std::string cmd)
{
    if ((int)cmd.find("search") > -1) return CLIENT_MSG_SEARCH;
    if ((int)cmd.find("ls") > -1) return CLIENT_CMD_LIST;
    if ((int)cmd.find("add") > -1) return CLIENT_MSG_APPADD;
}

bool doRegister(BaseClient *bc)
{
    printf("| Registering.\n");
    bool flag = bc->register_account();
    return flag;
}

bool serve(BaseClient *bc)
{
    string cmd, arg1, arg2;
    int op;
    bool valid;

    bc->start_recv();

    while(true)
    {
        cout << ">> ";
        cin >> cmd;
        op = decodeCMD(cmd);

        switch(op)
        {
        case CLIENT_MSG_SEARCH:
            cout << " | Request to search all users." << endl;
            if(bc->sendRequest(op)) {
                for (auto key : bc->all_users.getMemberNames())
                    cout << key << " " << bc->all_users[key] << endl;
            } else
                cout << " | Search request failed." << endl;
            break;
        case CLIENT_CMD_LIST:
            for (auto user : bc->friends)
                cout << user << endl;
            break;
        case CLIENT_MSG_APPADD:
            cin >> arg1;
            cout << " | Apply to add " << arg1 << endl;
            valid = !bc->all_users[arg1].isNull();
            if(!valid) cout << " | User not found, request terminated." << endl;
            else if(bc->sendRequest(op))
            {
                cout << " | Application accepted, friend " << arg1 << " added" << endl;
                bc->friends[arg1] = bc->all_users[arg1];
            }
            else cout << " | Application rejected." << endl;
            break;
        }
        
    }
}

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
        cout << ">> ";
        // First : login or register
        while(true)
        {
            cin >> cmd;
            //cout << (int)cmd.find("fastlogin") << " " << (int)cmd.find("login") <<endl;
            if ((int)cmd.find("fl") > -1) break;
            else if ((int)cmd.find("login") > -1) {
                cin >> arg1 >> arg2 >> arg3;
                bc->getClientConfig()->account = arg1;
                bc->getClientConfig()->passwd = arg2;
                bc->getClientConfig()->name = arg3;
                break;
            } else if ((int)cmd.find("reg") > -1) {
                cin >> arg1 >> arg2 >> arg3;
                bc->getClientConfig()->account = arg1;
                bc->getClientConfig()->passwd = arg2;
                bc->getClientConfig()->name = arg3;
                printf("| Connecting to server.\n");

                state ++;
                ctrl_flag[state] = bc->connectServer();
                if(!ctrl_flag[state])
                {
                    printf("| Connection to server failed.\n");
                    return 0;
                }
                doRegister(bc);
                break;
            }
        }
        printf("| Connecting to server.\n");

        state ++;
        ctrl_flag[state] = bc->connectServer();
        if(!ctrl_flag[state])
        {
            printf("| Connection to server failed.\n");
            return 0;
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

        // The circulation of login state
        serve(bc);
    }
    return 0;
}