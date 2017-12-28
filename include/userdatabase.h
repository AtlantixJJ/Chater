#ifndef USERDATABASE_H_
#define USERDATABASE_H_

#include "json/json.h"
#include "clientstatus.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

class ClientStatus;

/**
 * Using JSON directly for user status control
 **/
class UserDataBase
{
public:
    UserDataBase();
    UserDataBase(std::string fname);

    void writeToFile();

    void recheck();

    void setFriend(string p1, string p2);
    void setPeer(string account, string peer_ac, ClientStatus *cc);
    void setStatus(string account, int status, ClientStatus *cc);
    string getAllFriends(string ac);
    string getAllUsers();
    const Json::Value& getRoot() const {return root;}
    
    bool loginVerify(string account, string passwd);    
    void registerUser(string account, string passwd, string name);
    Json::Value findUser(string account);

private:
    void fromJson();
    void toJson();

private:
    std::vector<Json::Value> user_list;

    std::string file_name;

    Json::Value root;

};

#endif