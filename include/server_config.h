#include <iostream>
#include <string>
#include "json/json.h"

class ServerConfig
{
public:
    ServerConfig();
    ServerConfig(string fname);

    bool writeToFile();

private:
    bool has_file;
    string file_name;
    Json::Value root;
};