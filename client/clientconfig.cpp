#include "clientconfig.h"

ClientConfig::ClientConfig():
    name("Xu Jianjin"),passwd("12345"),account("atlantix")
{

}

ClientConfig::ClientConfig(string fname)
{
    file_name = fname;
}

void ClientConfig::setFieldValueFromJson()
{
    name            = root["name   "].asString();
    account         = root["account"].asString();
    passwd          = root["passwd "].asString();
}

void ClientConfig::saveFieldValueToJson()
{
    root["name"]            = Json::Value(name);
    root["account"]         = Json::Value(account);
    root["passwd"]          = Json::Value(passwd);
}

bool ClientConfig::writeToFile()
{
    saveFieldValueToJson();
    ofstream fout(file_name);
    fout << root;
    return true;
}