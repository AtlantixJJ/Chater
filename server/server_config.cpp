#include "json/json.h"
#include "server_config.h"
#include <fstream>
#include <string>
using namespace std;

ServerConfig::ServerConfig(string fname)
{
    has_file = true;
    file_name = fname;
}

ServerConfig::ServerConfig()
{
    root["ip"] = Json::Value("127.0.0.1");
    root["port"] = Json::Value(10222);
    root["max_thr"] = Json::Value(128);

    has_file = false;
}

bool ServerConfig::writeToFile()
{
    if (has_file == false)
    {
        return false;
    }
    Json::StyledWriter sw;
    std::ofstream fout(file_name);
    fout << sw.
    return true;
}