#include "userdatabase.h"
#include "common.h"
#include <fstream>
#include <iostream>
using namespace std;

UserDataBase::UserDataBase()
{
}

UserDataBase::UserDataBase(std::string fname)
{
    file_name = fname;
    ifstream fin(file_name);
    fin >> root;
}

void UserDataBase::fromJson()
{

}

void UserDataBase::toJson()
{
    for (int i = 0; i < user_list.size(); i++)
    {
        string s;
        char temp[32];
        sprintf(temp, "%d", i);
        root[s=temp] = user_list[i];
    }
}

void UserDataBase::writeToFile()
{
    //toJson();
    std::ofstream fout(file_name);
    fout << root;
}

bool UserDataBase::loginVerify(string account, string passwd)
{
    Json::Value s = root[account];

    if (s == Json::Value())
        return false;

    if (s.asString() == passwd)
        return true;
    else
        return false;
}

/// TODO : register exception
void UserDataBase::registerUser(string account, string passwd)
{
    root[account]       = Json::Value(account)      ;
    root[passwd]        = Json::Value(passwd)       ;
    root["status"]      = Json::Value(CLIENT_ONLINE);
}