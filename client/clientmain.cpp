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
    if ((int)cmd.find("chat") > -1) return CLIENT_CMD_CHAT;
    if ((int)cmd.find("exit") > -1) return CLIENT_CMD_EXIT;
}

bool doRegister(BaseClient *bc)
{
    printf(" | [CMD] Registering.\n");
    bool flag = bc->register_account();
    return flag;
}

bool requestUserList(BaseClient *bc)
{
    cout << " | [CMD] Request to search all users." << endl;

    if(bc->sendRequest(CLIENT_MSG_SEARCH, ""))
    {
        for (auto key : bc->all_users.getMemberNames())
            cout << key << " : " << bc->all_users[key] << endl;
        return true;
    }
    else
    {
        cout << " | [CMD] Search request failed." << endl;
        return false;
    }
}

bool requestAddFriend(BaseClient *bc, string account)
{
    cout << " | [CMD] Apply to add " << account << endl;
    bool valid = !bc->all_users[account].isNull();
    bool flag = true;
    if(!valid)
    {
        cout << " | [CMD] User not found, request terminated." << endl;
        flag = false;
    }
    else if(bc->sendRequest(CLIENT_MSG_APPADD, account))
    {
        cout << " | [CMD] Application accepted, friend " << account << " added" << endl;
        bc->friends[account] = bc->all_users[account];
        flag = true;
    }
    else
    {
        cout << " | [CMD] Application rejected." << endl;
        flag = false;
    }
    return flag;
}

void start_chat(BaseClient *bc, string account)
{
    if (bc->friends[account].isNull()) {
        printf(" | [CMD] Friend [ %s ] not found", account.c_str());
        return;
    }

    bc->sendMessage(CLIENT_MSG_CHAT, account.c_str());

    bc->peer_ac = account;

    bc->start_chat();
}

/// Request some necessary information from server
bool init(BaseClient *bc)
{
    bool flag;
    //sleep(100);
    bc->start_recv();
    flag = requestUserList(bc);
    return true;
}

bool serve(BaseClient *bc)
{
    string cmd, arg1, arg2;
    int op;
    bool valid;

    cout << " | [CMD] Hi [ " << bc->getClientConfig()->getName() <<
        " ], account [ " << bc->getClientConfig()->getAccount() << " ]\n";

    while(true)
    {
        cout << ">> ";
        cin >> cmd;
        op = decodeCMD(cmd);

        if(bc->isChatting()) bc->start_chat();

        switch(op)
        {
        case CLIENT_MSG_SEARCH: requestUserList(bc); break;
        case CLIENT_CMD_LIST:
            cout << " | [CMD] Friends list: " << endl;
            for (auto key : bc->friends.getMemberNames())
                cout << key << " : " << bc->friends[key] << endl;
            break;
        case CLIENT_MSG_APPADD:
            cin >> arg1;
            requestAddFriend(bc, arg1);
            break;
        case CLIENT_CMD_CHAT:
            cin >> arg1;
            start_chat(bc, arg1);
            break;
        case CLIENT_CMD_EXIT:
            cout << " | [CMD] Exit login." << endl;
            return true;
            break;
        }
        
    }
}

int main()
{
    BaseClient *bc = new BaseClient("server_config.json", "client_config.json");
    string cmd, arg1, arg2, arg3;
    bool ctrl_flag[64];
    char buf[32];
    int state = 0;

    printf(" | [CMD] Simple client by 2015011313.\n");
    printf(" | [CMD] Server %s:%d\n", bc->getServerConfig()->getServerIP(),
        bc->getServerConfig()->getServerPort());

    while(true)
    {
        state = 0;
        // First : login or register
        while(true)
        {
            cout << ">> ";
            cin >> cmd;
            //cout << (int)cmd.find("fastlogin") << " " << (int)cmd.find("login") <<endl;
            if ((int)cmd.find("fl") > -1) {
                string tmp = "client";
                buf[0] = cmd[2]; buf[1] = '\0';
                tmp += buf; tmp += "_config.json";
                cout << " | [CMD] Using " << tmp << " as configuration file." << endl;
                bc->getClientConfig()->fromFile(tmp);
                break;
            } else if ((int)cmd.find("login") > -1) {
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
                printf(" | [CMD] Connecting to server.\n");

                state ++;
                ctrl_flag[state] = bc->connectServer();
                if(!ctrl_flag[state])
                {
                    printf(" | [CMD] Connection to server failed.\n");
                    return 0;
                }
                doRegister(bc);
                break;
            }
        }
        printf(" | [CMD] Connecting to server.\n");

        state ++;
        ctrl_flag[state] = bc->connectServer();
        if(!ctrl_flag[state])
        {
            printf(" | [CMD] Connection to server failed.\n");
            return 0;
        }
        printf(" | [CMD] Login to server.\n");
        
        state ++;
        ctrl_flag[state] = bc->login();
        if(!ctrl_flag[state])
        {
            printf(" | [CMD] Login to server failed.\n");
            continue;
        }

        printf(" | [CMD] Login ackwonledged.\n");

        init(bc);

        // The circulation of login state
        serve(bc);
    }
    return 0;
}