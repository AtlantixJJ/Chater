#ifndef USERDATABASE_H_
#define USERDATABASE_H_

#include "json/json.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

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

    void setStatus(string account, int status);
    string getAllUsers();
    bool loginVerify(string account, string passwd);    
    void registerUser(string account, string passwd, string name);
    string findUser(string account);

private:
    void fromJson();
    void toJson();

private:
    std::vector<Json::Value> user_list;

    std::string file_name;

    Json::Value root;

};

#endif