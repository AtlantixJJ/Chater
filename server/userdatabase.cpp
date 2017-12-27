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
    cout << root << endl;
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
    cout << account << " " << passwd << endl;
    for(auto item : root)
    {
        cout << item["account"] << " " << item["passwd"] << endl;
        if (item["account"].asString() == account &&
            item["passwd"].asString() == passwd)
            return true;
    }
    return false;
}

string UserDataBase::findUser(string account)
{
    char temp[32];
    for(int i = 0; i < root.size(); i++)
    {
        sprintf(temp, "%d", i);
        Json::Value cur = root[temp];
        if (cur["account"].asString() == account)
            return string(temp);
    }
    sprintf(temp, "%d", root.size());
    return string(temp);
}

void UserDataBase::registerUser(string account, string passwd)
{
    string key = findUser(account);

    Json::Value node;
    node["account"]       = Json::Value(account)            ;
    node["passwd"]        = Json::Value(passwd)             ;
    node["status"]        = Json::Value(CLIENT_VERIFITED)   ;
    
    root[key] = node;
    writeToFile();
}