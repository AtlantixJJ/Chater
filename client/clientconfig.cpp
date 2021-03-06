#include "clientconfig.h"
#include <sstream>

ClientConfig::ClientConfig():
    name("Xu Jianjin"),passwd("12345"),account("atlantix")
{

}

ClientConfig::ClientConfig(string fname)
{
    fromFile(fname);
}

void ClientConfig::setFieldValueFromJson()
{
    name            = root["name"   ].asString();
    account         = root["account"].asString();
    passwd          = root["passwd" ].asString();
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

string ClientConfig::getLoginContent()
{
    saveFieldValueToJson();
    std::ostringstream stream;

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // No identation for message encoding
    std::string document = Json::writeString(wbuilder, root) + "\n\n\0\0";

    return document;
}

void ClientConfig::fromFile(string fname)
{
    file_name = fname;
    std::ifstream fin(fname);
    fin >> root;

    setFieldValueFromJson();
}