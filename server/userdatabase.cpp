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
    //mcout << root << endl;
}

void UserDataBase::fromJson()
{
    

}

void UserDataBase::toJson()
{

}

void UserDataBase::recheck()
{
    for(auto key : root.getMemberNames())
    {
        root[key]["status"] = Json::Value(CLIENT_OFFLINE);
        root[key]["peer"]   = Json::Value("{}"          );
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
    //cout << account << " " << passwd << endl;
    if (!root[account].isNull())
        if (root[account]["passwd"].asString() == passwd)
            return true;
    return false;
}

void UserDataBase::setFriend(string p1, string p2)
{
    Json::Value f1, f2;
    f1["name"]      = root[p2]["name"];
    f1["status"]    = root[p2]["status"];
    f2["name"]      = root[p1]["name"];
    f2["status"]    = root[p1]["status"];

    root[p1]["friends"][p2]     = f1;
    root[p2]["friends"][p1]     = f2;
}

bool UserDataBase::isOnline(string ac)
{
    return (root[ac]["status"].asInt() == CLIENT_VERIFIED);
}

string UserDataBase::getAllFriends(string ac)
{
    Json::Value node = root[ac]["friends"];
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding
    std::string document = Json::writeString(wbuilder, node) + "\0\n\0";
    return document;
}

void UserDataBase::setPeer(string account, string peer_ac, ClientStatus *cc)
{
    Json::Value PeerNode;
    PeerNode["account"] = Json::Value(peer_ac);
    PeerNode["client_status"]  = Json::Value((Json::Int64)cc);
    root[account]["peer"] = PeerNode;
}

void UserDataBase::setStatus(string account, int status, ClientStatus *cc)
{
    cout << root[account] << endl;
    if (!root[account].isNull())
    {
        root[account]["status"] = Json::Value(status);
        root[account]["client_status"] = Json::Value((Json::Int64)cc);
    }
}

Json::Value UserDataBase::findUser(string account)
{
    return root[account];
    /*
    char temp[32];
    for(int i = 0; i < root.size(); i++)
    {
        sprintf(temp, "%d", i);
        Json::Value cur = root[temp];
        if (cur["account"].asString() == account)
            return string(temp);
    }
    sprintf(temp, "%d", root.size());
    */
    //Json::Value item = cur["account"];

}

void UserDataBase::registerUser(string account, string passwd, string name)
{
    Json::Value node;
    //node["account"]       = Json::Value(account)            ;
    node["name"]          = Json::Value(name)               ;
    node["passwd"]        = Json::Value(passwd)             ;
    node["off_msg"]       = Json::Value("{}")               ;

    node["status"]        = Json::Value(CLIENT_VERIFIED)   ;

    // chat peer
    node["peer"]          = Json::Value("")                 ;

    root[account] = node;
    writeToFile();
}

string UserDataBase::getAllUsers()
{
    Json::Value names;
    for(auto key : root.getMemberNames())
    {
        Json::Value node;
        cout << root[key] << endl;
        node["status"]  = root[key]["status"];
        node["name"]    = root[key]["name"];
        names[key]      = node;
    }

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding
    std::string document = Json::writeString(wbuilder, names) + "\0\n\0";
    return document;
}